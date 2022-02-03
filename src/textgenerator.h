#ifndef TEXTGENERATOR_H
#define TEXTGENERATOR_H

#include <QMatrix>
#include <QObject>
#include <QVariantList>
#include <QRandomGenerator>
#include <QQmlApplicationEngine>
#include <QElapsedTimer>
#include <QTimer>

#include "dynamicsettings.h"

class Matrix : public QVector<quint8>
{
public:
    explicit Matrix(const int rows = 0, const int cols = 0);

    quint8 &operator()(int i, int j);
    quint8 operator()(int i, int j) const;

    int rows() const;
    void setRows(int newRows);

    int cols() const;
    void setCols(int newCols);

    void appendRows(const Matrix &rows);
    void clearRows();

private:
    int _rows;
    int _cols;
};

class TextGenerator : public QObject
{
    Q_OBJECT

    // text generator output
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(double measuredFps READ measuredFps NOTIFY measuredFpsChanged)

public:
    explicit TextGenerator(DynamicSettings &sttgs, QObject *parent = nullptr);
    ~TextGenerator();

    // DRAWING
    void repaintCharMatrix();
    Matrix repaintCharMatrixRows(int start, int end, Matrix &src);
    QString text();

    int convolveNeighbours(int i, int j, Matrix &src);
    double measuredFps() const;

signals:
    void textChanged();
    void measuredFpsChanged();

private:
    // settings
    float _fps;
    float _stepChances[2];
    int _numResets;
    QString _charContext;

    DynamicSettings &_settings;
    void onSettingChanged(const QString &setting);

    // the 'images' (two buffers to avoid reallocation)
    Matrix _charbuf0;
    Matrix _charbuf1;
    void initBuffers();

    // Animation stuff
    int mFrameId;
    QTimer mTimer;

    // Random Generation
    int distributedChoice(float dist[2], int size);
    int makeStep() const;

    bool mRedrawing;
    QElapsedTimer mFpsTimer;
    double _redrawTime;
};

#endif // TEXTGENERATOR_H
