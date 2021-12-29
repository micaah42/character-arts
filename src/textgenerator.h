#ifndef TEXTGENERATOR_H
#define TEXTGENERATOR_H

#include <QMatrix>
#include <QObject>
#include <QVariantList>
#include <QRandomGenerator>
#include <QQmlApplicationEngine>
#include <QElapsedTimer>
#include <QTimer>

#include "applicationsettings.h"

typedef QList<QList<int>> Matrix;


class TextGenerator : public QObject {
    Q_OBJECT

    // text generator output
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(double measuredFps READ measuredFps NOTIFY measuredFpsChanged)

  public:
    explicit TextGenerator(ApplicationSettings& sttgs, QObject* parent = nullptr);
    ~TextGenerator();

    // DRAWING
    void repaintCharMatrix();
    Matrix repaintCharMatrixCols(int start, int end, Matrix &src);
    QString text();

    int convolveNeighbours(int i, int j, Matrix &src);
    double measuredFps() const;


signals:
    void textChanged();
    void measuredFpsChanged();


private:
    // settings
    ApplicationSettings& _sttgs;

    // the 'images' (two buffers to avoid reallocation)
    Matrix mCharImageA;
    Matrix mCharImageB;
    void initCharMaps();

    // Animation stuff
    int mFrameId;
    QTimer mTimer;

    // Random Generation
    int distributedChoice(float dist[2], int size);
    int makeStep() const;

    bool mRedrawing;
    QElapsedTimer mFpsTimer;
    double mMeasuredFps;
    int m_fps;
};

#endif // TEXTGENERATOR_H
