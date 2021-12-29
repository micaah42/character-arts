#include "applicationsettings.h"

#include <QDebug>
#include <QVariantList>

ApplicationSettings* ApplicationSettings::instance = nullptr;

ApplicationSettings* ApplicationSettings::I()
{
    if (ApplicationSettings::instance == nullptr)
        ApplicationSettings::instance = new ApplicationSettings();

    return ApplicationSettings::instance;
}

ApplicationSettings::ApplicationSettings(QObject *parent) : QObject(parent)
{
    init();
}

void ApplicationSettings::init()
{
    setCols(320);
    setRows(85);

    setFps(25);
    setCharContextAsString(" ..-~=+*><//|\\(){}08&$#");
    mNumResets = 150;
    setFontSize(8);

    setStepChances(QVariantList{1./3., 1./3., 1./.3});
}

double ApplicationSettings::fps() const
{
    return mFps; }

void ApplicationSettings::setFps(double fps) {
    if (fps <= 0) {
        qWarning() << "Cannot set fps to zero!";
        return;
    }

    mFps = fps;
    emit fpsChanged();
}

void ApplicationSettings::setCharContextAsString(const QString& chars) {
    if (chars.length() < 2) {
        qWarning() << "Cannot set char context, requires at least two chars!";
        return;
    }

    mCharContext.clear();
    for (QChar c: chars) {
        mCharContext.append(c);
    }

    emit charContextChanged();
}

double ApplicationSettings::fontSize() const
{
    return mFontSize;
}

void ApplicationSettings::setFontSize(double fontSize)
{
    if (fontSize <= 0) {
        qWarning() << "Cannot set font size to zero!";
        return;
    }

    mFontSize = fontSize;
    emit fontSizeChanged();
}

int ApplicationSettings::resetChance() const
{
    return mNumResets;
}

void ApplicationSettings::setResetChance(const int resetChance)
{
    mNumResets = resetChance;
}

const QList<QChar>& ApplicationSettings::charContext() const
{
    return mCharContext;
}

QString ApplicationSettings::charContextAsString() const {
    QString ret = "";
    for (auto c : mCharContext) {
        ret.append(c);
    }
    return ret;
}

void ApplicationSettings::setCharContext(const QList<QChar> &charContext)
{
    if (charContext.size() < 2) {
        qWarning() << "Cannot set char context" << charContext << ": At least one char is required!";
        return;
    }

    mCharContext = charContext;
    emit charContextChanged();
}

int ApplicationSettings::cols() const
{
    return mCols;
}

void ApplicationSettings::setCols(const int cols)
{
    if (cols < 1) {
        qWarning() << "Cannot set cols to" << cols << ": At least one col is required!";
        return;
    }

    mCols = cols;
    emit colsChanged();
}

int ApplicationSettings::rows() const {
    return mRows;
}

void ApplicationSettings::setRows(const int rows)
{
    if (rows < 1) {
        qWarning() << "Cannot set rows to" << rows << ": At least one row is required!";
        return;
    }

    mRows = rows;
    emit rowsChanged();
}

QVariantList ApplicationSettings::stepChances() {
    QVariantList result;
    result.append(mStepChances[0]);
    result.append(mStepChances[1] - mStepChances[0]);
    result.append(1. - mStepChances[1]);
    return result;
}

float ApplicationSettings::stepChance(int of) const
{
    Q_ASSERT(0 <= of && of < 3);
    return mStepChances[of];
}

void ApplicationSettings::setStepChances(QVariantList stepChances) {
    Q_ASSERT(stepChances.length() == 3);
    mStepChances[0] = stepChances[0].toDouble();
    mStepChances[1] = stepChances[1].toDouble() + mStepChances[0];
    qInfo() << "down:" << mStepChances[0] << "equal:" << mStepChances[1] - mStepChances[0] << "up:" << 1 - mStepChances[1];
    emit stepChancesChanged();
}

