#include "textgenerator.h"

#include <stdlib.h>
#include <QDebug>
#include <QVector>
#include <QFutureWatcher>
#include <QRandomGenerator>
#include <QtConcurrent/QtConcurrentRun>

#include "applicationsettings.h"

TextGenerator::TextGenerator(ApplicationSettings& sttgs, QObject* parent) : QObject(parent), _sttgs(sttgs)
{
    initCharMaps();
    mFrameId = 0;

    connect(&mTimer, &QTimer::timeout, this, &TextGenerator::repaintCharMatrix);
    mTimer.start();

    connect(&_sttgs, &ApplicationSettings::fpsChanged, this, [this]() { mTimer.setInterval(1000 / _sttgs.fps()); });
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

int TextGenerator::makeStep() const
{
    float randD = (float)rand() / (float)RAND_MAX;
    if (randD < _sttgs.stepChance(0))
        return 0;
    else if (randD < _sttgs.stepChance(1))
        return 1;
    else
        return 2;
}

void TextGenerator::repaintCharMatrix() {
    if(mRedrawing)
        return;
    else if (_sttgs.rows() == 0 || _sttgs.cols() == 0)
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
        int start = i * (_sttgs.cols() / numThreads);
        int end = i < numThreads - 1 ? (i + 1) * (_sttgs.cols() / numThreads) : _sttgs.cols();
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
    for (int i = 0; i < _sttgs.resetChance(); i++) {
        randX = rand() % _sttgs.cols();
        randY = rand() % _sttgs.rows();

        dst[randX][randY] = _sttgs.charContextAsString().size() - 1; // rand() % _sttgs.charContext().length();
    }

    emit textChanged();

}

Matrix TextGenerator::repaintCharMatrixCols(int start, int end, Matrix &src)
{
    Matrix ret;
    for (int i = start; i < end; i++) {
        ret.append(QList<int>());
        for (int j = 0; j < _sttgs.rows(); j++) {
            bool moveJ = rand() % 3 == 0 && j != 0;
            bool moveI = rand() % 4 == 0 && i != 0;

            int oldChar = src[i - moveI][j - moveJ];
            int step = makeStep() - 1;
            int newChar = oldChar + step;
            // if old char is edge char, instead of invalid transition we stay equal
            if (newChar > _sttgs.charContextAsString().size() - 1)
                newChar = _sttgs.charContextAsString().size() - 1;
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
    for (int j = 0; j < _sttgs.rows(); j++) {
        for (int i = 0; i < _sttgs.cols(); i++) {
            Q_ASSERT(outMap[i][j] >= 0 && outMap[i][j] < _sttgs.charContext().size());
            int val = outMap[i][j];
            result += _sttgs.charContext()[val];
        }
        result += '\n';
    }

    double newFps = 60000 / mFpsTimer.elapsed();
    mMeasuredFps = 0.5 * newFps + 0.5 * mMeasuredFps;
    emit measuredFpsChanged();
    mRedrawing = false;
    return result;
}

void TextGenerator::initCharMaps()
{
    mCharImageA.clear();
    mCharImageB.clear();

    for (int i = 0; i < _sttgs.cols(); i++) {
        QList<int> col;
        for (int j = 0; j < _sttgs.rows(); j++) {
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
