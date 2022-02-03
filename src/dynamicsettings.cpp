#include "dynamicsettings.h"

#include <QDebug>
#include <QLoggingCategory>

namespace  {
    Q_LOGGING_CATEGORY(self, "dynamicsettings");
}

DynamicSettings::DynamicSettings(QObject *parent)
    : QObject{parent}
{

}

const QVariantMap DynamicSettings::map()
{
    return _values;
}

QVariant DynamicSettings::value(const QString &setting)
{
    return _values[setting];
}

void DynamicSettings::createNewSetting(const QString &setting, const QVariant &defaultValue)
{
    // the setting should not have been registered already!
    if (_values.contains(setting)) {
        qCWarning(self) << "Setting created multiple times:" << setting;

        if (_values[setting].type() != defaultValue.type()) {
            qCCritical(self) << "Setting created with multiple types:" << _values[setting].typeName() << defaultValue.typeName();
        }
        return;
    }

    _values[setting] = defaultValue;
    _subscribers[setting] = QList<QVariant*>();
    emit valueChanged(setting);
    emit keysChanged();
}

void DynamicSettings::changeSetting(const QString &setting, QVariant value)
{
    if (!_values.contains(setting)) {
        qCCritical(self) << "Cannot change non-existent setting:" << setting;
        return;
    }
    if (!value.convert(_values[setting].type())) {
        qCCritical(self) << "Setting has incompatible type:" << _values[setting].typeName() << value.typeName();
        emit valueChanged(setting);
        return;
    }

    if (_values[setting] == value) {
        return;
    }

    qCDebug(self) << "setting changed: " << setting << "," << _values[setting] << "->" << value;
    _values[setting] = value;

    for (auto subscriber : qAsConst(_subscribers[setting])) {
        *subscriber = value;
    }

    emit valueChanged(setting);
}

void DynamicSettings::subscribeSetting(const QString &setting, QVariant &subscriber)
{
    if (!_values.contains(setting)) {
        qCCritical(self) << "Cannot subsribe to non-existent setting:" << setting;
        return;
    }

    if (_subscribers[setting].contains(&subscriber)) {
        qCWarning(self) << "Subscriber is already subscribed to this setting:" << &subscriber << setting;
        return;
    }

    _subscribers[setting].append(&subscriber);
    subscriber = _values[setting];
}

void DynamicSettings::unsubsribeSetting(const QString &setting, QVariant &subscriber)
{
    if (!_values.contains(setting)) {
        qCCritical(self) << "Cannot unsubsribe from non-existent setting:" << setting;
        return;
    }

    if (!_subscribers[setting].contains(&subscriber)) {
        qCWarning(self) << "Subscriber has never subscribed to this setting:" << &subscriber << setting;
        return;
    }

    _subscribers[setting].removeOne(&subscriber);
}

const QVariantList DynamicSettings::keys() const
{
    QVariantList keys;
    for (auto key : _values.keys()) {
        keys.append(key);
    }

    return keys;
}

bool DynamicSettings::isConvertable(const QString &setting, const QVariant &variant)
{
    return true;
}
