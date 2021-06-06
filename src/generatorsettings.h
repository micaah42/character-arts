#ifndef GENERATORSETTINGS_H
#define GENERATORSETTINGS_H

#include <QObject>
#include <QSettings>

#define STRINGIFY_CODE(CODE) QString($CODE)
class GeneratorSettings : public QObject
{
    Q_OBJECT
public:
    explicit GeneratorSettings(QObject *parent = nullptr);

    // make sure this list matches mParameterStrings!!!
    enum Parameter {
        param0,
        param1,
        param2,
        _count
    };

    // get the internal string representation
    QString valueString(const Parameter &param);

    // getter
    QVariant value(const Parameter &param);
    // setter
    QVariant setValue(const Parameter &param, const QVariant &variant);

signals:
    // signal
    void valueChanged(const Parameter &param);

private:
    // make sure this list matches Parameter Enum!!!
    const QStringList mParameterStrings = {
        "param0",
        "param1",
        "param2"
    };

    QSettings* mSettings;
};

#endif // GENERATORSETTINGS_H
