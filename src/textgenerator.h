#ifndef TEXTGENERATOR_H
#define TEXTGENERATOR_H

#include <QMatrix>
#include <QObject>
#include <QVariantList>
#include <QRandomGenerator>
#include <QQmlApplicationEngine>
#include <QElapsedTimer>
#include <QTimer>

typedef QList<QList<int>> Matrix;

class TextGenerator : public QObject {
    Q_OBJECT

    // text generator output
    Q_PROPERTY(QString text READ text NOTIFY textChanged)

    // settings
    Q_PROPERTY(int fps READ fps WRITE setFps NOTIFY fpsChanged)
    Q_PROPERTY(int cols READ cols WRITE setCols NOTIFY colsChanged)
    Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
    Q_PROPERTY(double fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(int resetChance READ resetChance WRITE setResetChance NOTIFY resetChanceChanged)
    Q_PROPERTY(QVariantList stepChances READ stepChances WRITE setStepChances NOTIFY stepChancesChanged)
    Q_PROPERTY(QString charContext READ charContext WRITE setCharContext NOTIFY charContextChanged)
    Q_PROPERTY(double measuredFps READ measuredFps NOTIFY measuredFpsChanged)

    // char filter: move charcontext to own class

  public:
    explicit TextGenerator(QObject* parent = nullptr);
    ~TextGenerator();

    // SETTINGS
    int fps() const;
    void setFps(int fps);

    int cols() const;
    void setCols(const int cols);

    int rows() const;
    void setRows(const int rows);


    QVariantList stepChances();
    void setStepChances(QVariantList stepChances);
    void setStepChances(float downChance, float upChance);

    int resetChance() const;
    void setResetChance(const int resetChance);

    QString charContext() const;
    void setCharContext(const QString& chars);

    // DRAWING
    void repaintCharMatrix();
    Matrix repaintCharMatrixCols(int start, int end, Matrix &src);
    QString text();

    int convolveNeighbours(int i, int j, Matrix &src);

    double fontSize() const;
    void setFontSize(double fontSize);

    double measuredFps() const;

signals:
    void textChanged();
    void stepChancesChanged();
    void fontSizeChanged();

    void measuredFpsChanged();

    void fpsChanged();

    void colsChanged();

    void rowsChanged();

    void resetChanceChanged();

    void charContextChanged();

private:
    // Generation Parameters:
    int mNumResets;

    // image dimensions
    int mCols, mRows;
    // and font size
    double mFontSize;

    // the 'images' (two buffers to avoid reallocation)
    Matrix mCharImageA;
    Matrix mCharImageB;
    void initCharMaps();

    // interpretation of chars - basically a list that puts similar chars together
    QList<QChar> mCharContext;

    // step chances (wether the char goes up, down or stays the same within the
    // context)
    float mStepChances[2];

    // random reset chance (per char)
    double mResetChance;

    // Animation stuff
    int mFrameId;
    QTimer mTimer;

    // Random Generation
    int distributedChoice(float dist[2], int size);
    int makeStep(int before);

    bool mRedrawing;
    QElapsedTimer mFpsTimer;
    double mMeasuredFps;
    int m_fps;
};

// ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++
// ++++ Define the singleton type provider function (callback).
static QObject* themeSingletonProvider(QQmlEngine* engine, QJSEngine* scriptEngine) {
    Q_UNUSED(scriptEngine)

    TextGenerator* singletonTheme = new TextGenerator(engine);
    return singletonTheme;
}

#endif // TEXTGENERATOR_H
