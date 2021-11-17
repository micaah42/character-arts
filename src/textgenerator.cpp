#include "textgenerator.h"

#include <stdlib.h>
#include <QDebug>
#include <QFutureWatcher>
#include <QRandomGenerator>
#include <QVector>
#include <QtConcurrent/QtConcurrentMap>

TextGenerator::TextGenerator(const quint32& cols, const quint32& rows, QObject* parent) : QObject(parent) {
    mCols = cols;
    mRows = rows;
    initCharMaps();

    setFps(25);
    setCharContext(" ..-~=+*><//|\\(){}08&$#");
    mNumResets = 15;

    mFrameId = 0;
    setStepChances(1. / 3., 1. / 3.);
    connect(&mTimer, &QTimer::timeout, this, &TextGenerator::repaintCharMap);
    mTimer.start();
}

TextGenerator::~TextGenerator() {
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
    Matrix &newMap = (0 == mFrameId % 2) ? mCharImageA : mCharImageB;
    Matrix &oldMap = (0 == mFrameId % 2) ? mCharImageB : mCharImageA;

    QFutureWatcher<void> watcher;
    connect(&watcher, &QFutureWatcher<void>::finished, &watcher, [this, newMap]() {
        qDebug() << "done updating char maps!";
        // make random resets
        quint32 randX, randY;
        for (quint32 i = 0; i < mNumResets; i++) {
            randX = rand() % mCols;
            randY = rand() % mRows;
            // newMap[randX][randY].a = mCharContext.length() - 1; // rand() % mCharContext.length();
        }

        mFrameId++;
        emit textChanged();
    });

    QFuture<void> future = QtConcurrent::map(newMap, [this, oldMap](QList<MatrixEntry> col) {
        quint32 i = col.first().x;
        for (quint32 j = 0; j < mRows; j++) {
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
            quint8 oldChar = (rand() % 2) == 1 ? oldMap[neighbourX][neighbourY].a : oldMap[i][j].a;

            qint8 step = distributedChoice(mStepChances, 3) - 1;
            // if old char is edge char, instead of invalid transition we stay equal
            if (oldChar == 0 && step == -1)
                step = 0;
            else if (oldChar == mCharContext.length() - 1 && step == 1)
                step = 0;

            quint8 newChar = oldChar + step;
            Q_ASSERT(newChar < mCharContext.size());
            qDebug() << "set col[" << j << "]" << newChar;
            col[j].a = newChar;
            qDebug() << "done updating col";
        }
    });
    watcher.setFuture(future);
}

QString TextGenerator::text() {
    QString result = "";
    Matrix &outMap = (0 == mFrameId % 2) ? mCharImageB : mCharImageA;
    for (quint32 j = 0; j < mRows; j++) {
        for (quint32 i = 0; i < mCols; i++) {
            quint8 val = outMap[i][j].a;
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
    initCharMaps();
}

quint32 TextGenerator::rows() const { return mRows; }

void TextGenerator::setRows(const quint32& rows)
{
    mRows = rows;
    initCharMaps();
}

QVariantList TextGenerator::stepChances() {
    QVariantList result;
    result.append(mStepChances[0]);
    result.append(mStepChances[1]);
    return result;
}

void TextGenerator::setStepChances(QVariantList stepChances) {
    Q_ASSERT(stepChances.length() == 3);
    mStepChances[0] = stepChances[0].toDouble();
    mStepChances[1] = stepChances[1].toDouble() + mStepChances[0];
    emit stepChancesChanged();
}

void TextGenerator::setStepChances(float up, float down) {
    mStepChances[0] = up;
    mStepChances[1] = up + down;
    emit stepChancesChanged();
}

void TextGenerator::initCharMaps()
{
    mCharImageA.clear();
    mCharImageB.clear();

    for (quint32 i = 0; i < cols(); i++) {
        QList<MatrixEntry> col;
        for (quint32 j = 0; j < rows(); j++) {
            col.append(MatrixEntry(0, i, j));
        }
        mCharImageA.append(col);
        mCharImageB.append(col);
    }
}
