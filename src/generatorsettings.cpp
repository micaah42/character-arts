#include "generatorsettings.h"

GeneratorSettings::GeneratorSettings(QObject *parent) : QObject(parent)
{

}

QString GeneratorSettings::valueString(const GeneratorSettings::Parameter param)
{
    Q_ASSERT(param >= 0 && param < _count);
    return mParameterStrings[param];
}
