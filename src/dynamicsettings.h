#ifndef DYNAMICSETTINGS_H
#define DYNAMICSETTINGS_H

#include <QMap>
#include <QVariant>
#include <QObject>

class DynamicSettings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList keys READ keys NOTIFY keysChanged)

public:
    explicit DynamicSettings(QObject *parent = nullptr);

    Q_INVOKABLE const QVariantMap map();
    Q_INVOKABLE QVariant value(const QString &setting);

    Q_INVOKABLE void createNewSetting(const QString &setting, const QVariant &defaultValue);
    Q_INVOKABLE void changeSetting(const QString &setting, QVariant value);

    // int overloads
    Q_INVOKABLE void subscribeSetting(const QString &setting, QVariant& subsriber);
    Q_INVOKABLE void unsubsribeSetting(const QString &setting, QVariant& subscriber);

    // save current settings
    Q_INVOKABLE void saveSettings(const QString &filename);
    Q_INVOKABLE void loadSettings(const QString &filename);

    const QVariantList keys() const;

signals:
    void valueChanged(const QString &setting);
    void keysChanged();


private:
    bool isConvertable(const QString &setting, const QVariant &variant);

    QMap<QString, QVariant> _values;
    QMap<QString, QList<QVariant*>> _subscribers;
};

#endif // DYNAMICSETTINGS_H
