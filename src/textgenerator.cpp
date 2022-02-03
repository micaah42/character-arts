#include "textgenerator.h"

#include <stdlib.h>
#include <QDebug>
#include <QFutureWatcher>
#include <QLoggingCategory>
#include <QRandomGenerator>
#include <QVector>
#include <QtConcurrent/QtConcurrentRun>

#include "dynamicsettings.h"

namespace {
Q_LOGGING_CATEGORY(self, "textgenerator");
}

TextGenerator::TextGenerator(DynamicSettings &sttgs, QObject *parent)
    : QObject(parent), _settings(sttgs), _charbuf0(1, 1), _charbuf1(1, 1)
{
    mFrameId = 0;

    connect(&mTimer, &QTimer::timeout, this, &TextGenerator::repaintCharMatrix);
    mTimer.start();

    connect(&_settings, &DynamicSettings::valueChanged, this, &TextGenerator::onSettingChanged);
    _settings.createNewSetting("generator.fps", 25);
    _settings.createNewSetting("generator.rows", 85);
    _settings.createNewSetting("generator.cols", 320);
    _settings.createNewSetting("generator.numresets", 150);
    _settings.createNewSetting("generator.charcontext", " ..-~=+*><//|\\(){}08&$#");
    _settings.createNewSetting("generator.stepchances", QVariantList{1 / 3., 2. / 3.});
}

TextGenerator::~TextGenerator() {}

int TextGenerator::distributedChoice(float *dist, int size)
{
    float randD = (float) rand() / (float) RAND_MAX;
    if (randD < dist[0])
        return 0;
    else if (randD < dist[1])
        return 1;
    else
        return 2;
}

int TextGenerator::makeStep() const
{
    float randD = (float) rand() / (float) RAND_MAX;
    if (randD < _stepChances[0])
        return 0;
    else if (randD < _stepChances[1])
        return 1;
    else
        return 2;
}

void TextGenerator::repaintCharMatrix()
{
    if (mRedrawing)
        return;
    else if (_charbuf0.empty())
        return;
    else if (_charContext.size() < 3)
        return;
    else {
        mFpsTimer.start();
        mRedrawing = true;
    }

    Matrix &src = mFrameId % 2 ? _charbuf1 : _charbuf0;
    Matrix &dst = mFrameId % 2 ? _charbuf0 : _charbuf1;

    int numThreads = 2;
    QList<QFuture<Matrix>> futures;
    for (int i = 0; i < numThreads; i++) {
        int start = i * (src.rows() / numThreads);
        int end = (i + 1) * (src.rows() / numThreads);
        QFuture<Matrix> future = QtConcurrent::run(this,
                                                   &TextGenerator::repaintCharMatrixRows,
                                                   start,
                                                   std::min(end, src.rows()),
                                                   src);
        futures.append(future);
    }

    dst.clearRows();
    for (auto future : futures) {
        future.waitForFinished();
        dst.appendRows(future.result());
    }

    // make random resets
    int i, j;
    for (int k = 0; k < _numResets; k++) {
        i = rand() % dst.rows();
        j = rand() % dst.cols();

        dst(i, j) = _charContext.size() - 2; // rand() % _sttgs.charContext().length();
    }

    emit textChanged();
}

Matrix TextGenerator::repaintCharMatrixRows(int start, int end, Matrix &src)
{
    Matrix ret(end - start, src.cols());
    for (int i = 0; i < ret.rows(); i++) {
        for (int j = 0; j < ret.cols(); j++) {
            bool moveJ = rand() % 3 == 0 && j != 0;
            bool moveI = rand() % 4 == 0 && i != 0;

            int oldChar = src(start + i - moveI, j - moveJ);
            int step = makeStep() - 1;
            int newChar = oldChar + step;

            // if old char is edge char, instead of invalid transition we stay equal
            if (newChar == _charContext.size() - 1)
                newChar = _charContext.size() - 2;
            else if (newChar < 0)
                newChar = 0;

            ret(i, j) = newChar;
        }
    }

    return ret;
}

QString TextGenerator::text()
{
    QString result = "";
    Matrix &outMap = (mFrameId % 2) ? _charbuf0 : _charbuf1;
    mFrameId++;
    for (int i = 0; i < outMap.rows(); i++) {
        for (int j = 0; j < outMap.cols(); j++) {
            if (outMap(i, j) < 0) {
                outMap(i, j) = 0;
            }
            else if (outMap(i, j) > _charContext.size() - 2) {
                outMap(i, j) = _charContext.size() - 2;
            }
            int val = outMap(i, j);
            result += _charContext[val];
        }
        result += '\n';
    }

    _redrawTime = mFpsTimer.elapsed();
    emit measuredFpsChanged();
    mRedrawing = false;
    return result;
}

double TextGenerator::measuredFps() const
{
    return _redrawTime;
}

void TextGenerator::onSettingChanged(const QString &setting)
{
    auto const split = setting.split(".");
    if (split[0] != "generator") {
        return;
    }

    auto const &name = split[1];
    if (name == "cols") {
        _charbuf0.setCols(_settings.value(setting).toInt());
        _charbuf1.setCols(_settings.value(setting).toInt());
    }
    else if (name == "rows") {
        _charbuf0.setRows(_settings.value(setting).toInt());
        _charbuf1.setRows(_settings.value(setting).toInt());
    }
    else if (name == "fps") {
        mTimer.setInterval(1000 / _settings.value(setting).toDouble());
    }
    else if (name == "numresets") {
        _numResets = _settings.value(setting).toInt();
    }
    else if (name == "charcontext") {
        _charContext = _settings.value(setting).toString();
    }
    else if (name == "stepchances") {
        auto stepchances = _settings.value(setting).toList();
        _stepChances[0] = stepchances[0].toDouble();
        _stepChances[1] = stepchances[1].toDouble();
    }
    else {
        qCCritical(self) << "unknown an unknown setting changed:" << setting
                         << _settings.value(setting);
    }
}

Matrix::Matrix(const int rows, const int cols)
    : QVector<quint8>(rows * cols, 0), _rows(rows), _cols(cols)
{}

quint8 &Matrix::operator()(int i, int j)
{
    Q_ASSERT(0 <= i && i < _rows);
    Q_ASSERT(0 <= j && j < _cols);
    return (*this)[i * _cols + j];
}

int Matrix::rows() const
{
    return _rows;
}

void Matrix::setRows(int newRows)
{
    _rows = newRows;
    fill(0, _rows * _cols);
}

int Matrix::cols() const
{
    return _cols;
}

void Matrix::setCols(int newCols)
{
    _cols = newCols;
    fill(0, _rows * _cols);
}

void Matrix::appendRows(const Matrix &rows)
{
    Q_ASSERT(rows.cols() == cols());
    _rows += rows.rows();
    this->append(rows);
}

void Matrix::clearRows()
{
    _rows = 0;
    this->clear();
}
