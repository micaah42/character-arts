#include "textgenerator.h"

#include <stdlib.h>
#include <QDebug>
#include <QVector>
#include <QFutureWatcher>
#include <QRandomGenerator>
#include <QtConcurrent/QtConcurrentRun>

TextGenerator::TextGenerator(QObject* parent) : QObject(parent) {
    mCols = 540;
    mRows = 140;
    initCharMaps();

    setFps(25);
    setCharContext(" ..-~=+*><//|\\(){}08&$#");
    mNumResets = 150;
    setFontSize(6);

    mFrameId = 0;
    setStepChances(1. / 3., 1. / 3.);


    connect(&mTimer, &QTimer::timeout, this, &TextGenerator::repaintCharMatrix);
    mTimer.start();
}

TextGenerator::~TextGenerator() {
}

int TextGenerator::distributedChoice(float* dist, int size) {
    float randD = (float)rand() / (float)RAND_MAX;
    if (randD < dist[0])
        return 0;
    else if (randD < dist[1])
        return 1;
    else
        return 2;
}

int TextGenerator::makeStep(int before)
{
    int ret;
    float randD = QRandomGenerator(time(0)).generateDouble();
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

int TextGenerator::fps() const { return 1000 / mTimer.interval(); }

void TextGenerator::setFps(int fps) {
    if (fps < 1) {
        fps = 1;
    }
    mTimer.setInterval(1000 / fps);

    emit fpsChanged();
}

void TextGenerator::setCharContext(const QString& chars) {
    if (chars.length() < 2)
        qFatal("too few chars");

    mCharContext.clear();
    for (QChar c: chars) {
        mCharContext.append(c);
    }
}

void TextGenerator::repaintCharMatrix() {
    if(mRedrawing)
        return;
    else if (mCols == 0 || mRows == 0)
        return;
    else {
        mFpsTimer.start();
        mRedrawing = true;
    }

    Matrix &dst = mFrameId % 2 ? mCharImageA : mCharImageB;
    Matrix &src = mFrameId % 2 ? mCharImageB : mCharImageA;

    int numThreads = 2;
    QList<QFuture<Matrix>> futures;
    for (int i = 0; i < numThreads; i++) {
        int start = i * (mCols / numThreads);
        int end = i < numThreads - 1 ? (i + 1) * (mCols / numThreads) : mCols;
        QFuture<Matrix> future = QtConcurrent::run(this, &TextGenerator::repaintCharMatrixCols, start, end, src);
        futures.append(future);
    }

    dst.clear();
    for (auto future : futures) {
        future.waitForFinished();
        dst.append(future.result());
    }

    // make random resets
    int randX, randY;
    for (int i = 0; i < mNumResets; i++) {
        randX = rand() % mCols;
        randY = rand() % mRows;

        dst[randX][randY] = mCharContext.size() - 1; // rand() % mCharContext.length();
    }

    emit textChanged();

}

Matrix TextGenerator::repaintCharMatrixCols(int start, int end, Matrix &src)
{
    Matrix ret;
    for (int i = start; i < end; i++) {
        ret.append(QList<int>());
        for (int j = 0; j < mRows; j++) {
            bool moveJ = rand() % 3 == 0 && j != 0;
            bool moveI = rand() % 4 == 0 && i != 0;

            int oldChar = src[i - moveI][j - moveJ];
            int step = distributedChoice(mStepChances, 3) - 1;
            int newChar = oldChar + step;
            // if old char is edge char, instead of invalid transition we stay equal
            if (newChar > mCharContext.size() - 1)
                newChar = mCharContext.size() - 1;
            else if (newChar < 0)
                newChar = 0;

            ret.last().append(newChar);
        }
    }

    return ret;
}

QString TextGenerator::text() {
    QString result = "";
    Matrix &outMap = (mFrameId % 2) ? mCharImageA : mCharImageB;
    mFrameId++;
    for (int j = 0; j < mRows; j++) {
        for (int i = 0; i < mCols; i++) {
            Q_ASSERT(outMap[i][j] >= 0 && outMap[i][j] < mCharContext.size());
            int val = outMap[i][j];
            result += mCharContext[val];
        }
        result += '\n';
    }

    double newFps = 60000 / mFpsTimer.elapsed();
    mMeasuredFps = 0.5 * newFps + 0.5 * mMeasuredFps;
    emit measuredFpsChanged();
    mRedrawing = false;
    return result;
}

double TextGenerator::fontSize() const { return mFontSize; }

void TextGenerator::setFontSize(double fontSize) {
    mFontSize = fontSize;
    emit fontSizeChanged();
}

int TextGenerator::resetChance() const { return mNumResets; }

void TextGenerator::setResetChance(const int resetChance) { mNumResets = resetChance; }

QString TextGenerator::charContext() const {
    QString ret = "";
    for (auto c : mCharContext) {
        ret.append(c);
    }
    return ret;
}

int TextGenerator::cols() const { return mCols; }

void TextGenerator::setCols(const int cols)
{
    if (cols < 1) {
        mCols = 1;
    }
    else
        mCols = cols;

    initCharMaps();
    emit colsChanged();
}

int TextGenerator::rows() const { return mRows; }

void TextGenerator::setRows(const int rows)
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
    qInfo() << "down:" << mStepChances[0] << "equal:" << mStepChances[1] - mStepChances[0] << "up:" << 1 - mStepChances[1];
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

    for (int i = 0; i < cols(); i++) {
        QList<int> col;
        for (int j = 0; j < rows(); j++) {
            col.append(0);
        }
        mCharImageA.append(col);
        mCharImageB.append(col);
    }
}

double TextGenerator::measuredFps() const
{
    return mMeasuredFps;
}
