#include "textgenerator.h"

#include <QDebug>
#include <QRandomGenerator>
#include <QVector>
#include <stdlib.h>

TextGenerator::TextGenerator(const quint32& cols, const quint32& rows, QObject* parent) : QObject(parent) {
    mCols = cols;
    mRows = rows;

    mCharImageA = new quint8[cols * rows];
    mCharImageB = new quint8[cols * rows];
    for (quint16 i = 0; i < mCols * mRows; i++)
        mCharImageA[i] = 0;

    setFps(25);
    setCharContext(" ..-~=+*><//|\\(){}08&$#");
    mNumResets = 15;

    mFrameId = 0;
    setStepChances(1. / 3., 1. / 3.);
    connect(&mTimer, &QTimer::timeout, this, &TextGenerator::repaintCharMap);
    mTimer.start();
}

TextGenerator::~TextGenerator() {
    delete[] mCharImageA;
    delete[] mCharImageB;
}

quint8 TextGenerator::distributedChoice(float dist[3], quint8 size) {
    float randD = mRandomGenerator.generateDouble();
    quint8 minPossible, maxPossible, mid;
    minPossible = 0;
    maxPossible = size;
    while (minPossible != maxPossible) {
        mid = (minPossible + maxPossible) / 2;
        if (randD > dist[mid])
            minPossible = mid + 1;
        else
            maxPossible = mid;
        // qDebug() << dist[minPossible] << "<=" << randD << "<=" << dist[maxPossible - 1];
    }
    return minPossible;
}

quint8 TextGenerator::makeStep(quint8 &before)
{
    quint8 ret;
    float randD = mRandomGenerator.generateDouble();
    if (randD < mStepChances[0])
        ret = before + 1;
    else if (randD < mStepChances[1])
        ret = before - 1;
    else
        ret = before;

    if (ret < 0)
        ret = 0;
    else if (ret >= mCharContext.size())
        ret = mCharContext.size() - 1;

    return ret;
}

quint8 TextGenerator::fps() const { return 1000 / mTimer.interval(); }

void TextGenerator::setFps(quint8 fps) { mTimer.setInterval(1000 / fps); }

void TextGenerator::setCharContext(const QString& chars) {
    if (chars.length() < 2)
        qFatal("too few chars");

    mCharContext.clear();
    for (quint16 i = 0; i < chars.length(); i++) {
        mCharContext.push_back(chars[i].toLatin1());
    }
}

void TextGenerator::repaintCharMap() {
    quint8* newMap = (0 == mFrameId % 2) ? mCharImageA : mCharImageB;
    quint8* oldMap = (0 == mFrameId % 2) ? mCharImageB : mCharImageA;

    //    for (quint32 i = 0; i < mCols * mRows; i++)
    //        newMap[i] = 0;

    // For each pixel
    for (quint32 j = 0; j < mRows; j++) {
        for (quint32 i = 0; i < mCols; i++) {

            // randomly choose a neighbour
            bool neighbour_right, neighbour_down;

            // up/down
            if (j == 0)
                neighbour_down = true;
            else if (j == (mRows - 1))
                neighbour_down = false;
            else
                neighbour_down = (rand() % 2) == 1;

            // left/right
            if (i == 0)
                neighbour_right = true;
            else if (i == (mCols - 1))
                neighbour_right = false;
            else
                neighbour_right = (rand() % 2) == 1;

            quint32 neighbourX = neighbour_right ? i + 1 : i - 1;
            quint32 neighbourY = neighbour_down ? j + 1 : j - 1;

            // apply transition to neighbour
            quint8 oldChar = (rand() % 2) == 1 ? oldMap[neighbourX + neighbourY * mCols] : oldMap[i + j * mCols];
            quint8 newChar;

            quint32 trans = distributedChoice(mStepChances, 3);
            // if old char is edge char, instead of invalid transition we stay equal
            if (oldChar == 0 && trans == 0)
                trans = 2;
            else if (oldChar == mCharContext.length() - 1 && trans == 2)
                trans = 2;

            newChar = oldChar + trans - 1;
            newMap[i + j * mCols] = newChar;
        }
    }

    // make random resets
    quint32 randX, randY;
    for (quint32 i = 0; i < mNumResets; i++) {
        randX = rand() % mCols;
        randY = rand() % mRows;
        newMap[randX + randY * mCols] = mCharContext.length() - 1; // rand() % mCharContext.length();
    }

    mFrameId++;
    emit textChanged();
}

QString TextGenerator::text() {
    QString result = "";
    quint8* outMap = (0 == mFrameId % 2) ? mCharImageB : mCharImageA;
    for (quint32 j = 0; j < mRows; j++) {
        for (quint32 i = 0; i < mCols; i++) {
            quint8 val = outMap[i + j * mCols];
            result += mCharContext[val];
        }
        result += '\n';
    }

    return result;
}

double TextGenerator::fontSize() const { return mFontSize; }

void TextGenerator::setFontSize(double fontSize) {
    mFontSize = fontSize;
    emit fontSizeChanged();
}

quint32 TextGenerator::resetChance() const { return mNumResets; }

void TextGenerator::setResetChance(const quint32& resetChance) { mNumResets = resetChance; }

QString TextGenerator::charContext() const { return mCharContext; }

quint32 TextGenerator::cols() const { return mCols; }

void TextGenerator::setCols(const quint32& cols)
{
    mCols = cols;

    delete []  mCharImageA;
    mCharImageA = new quint8[mCols * mRows];
    delete [] mCharImageB;
    mCharImageB = new quint8[mCols * mRows];
}

quint32 TextGenerator::rows() const { return mRows; }

void TextGenerator::setRows(const quint32& rows)
{
    mRows = rows;

    delete []  mCharImageA;
    mCharImageA = new quint8[mCols * mRows];
    delete [] mCharImageB;
    mCharImageB = new quint8[mCols * mRows];
}


QVariantList TextGenerator::stepChances() {
    QVariantList result;
    result.append(mStepChances[0]);
    result.append(mStepChances[1]);
    result.append(mStepChances[2]);
    return result;
}

void TextGenerator::setStepChances(QVariantList stepChances) {
    Q_ASSERT(stepChances.length() == 3);
    mStepChances[0] = stepChances[0].toDouble();
    mStepChances[1] = stepChances[1].toDouble() + mStepChances[0];
    mStepChances[2] = stepChances[2].toDouble() + mStepChances[1];
    emit stepChancesChanged();
}

void TextGenerator::setStepChances(float up, float down) {
    mStepChances[0] = up;
    mStepChances[1] = up + down;
    emit stepChancesChanged();
}
