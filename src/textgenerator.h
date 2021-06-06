#ifndef TEXTGENERATOR_H
#define TEXTGENERATOR_H

#include <QObject>
#include <QQmlApplicationEngine>
#include <QRandomGenerator>
#include <QTimer>
#include <QVariantList>
#include <QVector>

class TextGenerator : public QObject {
    Q_OBJECT

    // text generator output
    Q_PROPERTY(QString text READ text NOTIFY textChanged)

    // settings
    Q_PROPERTY(int fps READ fps WRITE setFps)
    Q_PROPERTY(int cols READ cols WRITE setCols)
    Q_PROPERTY(int rows READ rows WRITE setRows)
    Q_PROPERTY(double fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(quint32 resetChance READ resetChance WRITE setResetChance)
    Q_PROPERTY(QVariantList stepChances READ stepChances WRITE setStepChances NOTIFY stepChancesChanged)
    Q_PROPERTY(QString charContext READ charContext WRITE setCharContext)

    // char filter: move charcontext to own class

  public:
    explicit TextGenerator(const quint32& cols, const quint32& rows, QObject* parent = nullptr);
    ~TextGenerator();

    // SETTINGS
    quint8 fps() const;
    void setFps(quint8 fps);

    quint32 cols() const;
    void setCols(const quint32& cols);

    quint32 rows() const;
    void setRows(const quint32& rows);

    QVariantList stepChances();
    void setStepChances(QVariantList stepChances);
    void setStepChances(float downChance, float upChance);

    quint32 resetChance() const;
    void setResetChance(const quint32& resetChance);

    QString charContext() const;
    void setCharContext(const QString& chars);

    // DRAWING
    void repaintCharMap();
    QString text();

    double fontSize() const;
    void setFontSize(double fontSize);

  signals:
    void textChanged();
    void stepChancesChanged();
    void fontSizeChanged();

  private:
    // Generation Parameters:
    quint32 mNumResets;

    // image dimensions
    quint32 mCols, mRows;
    // and font size
    double mFontSize;

    // the 'images' (two buffers to avoid reallocation)
    quint8* mCharImageA;
    quint8* mCharImageB;

    // interpretation of chars - basically a list that puts similar chars together
    QString mCharContext;

    // step chances (wether the char goes up, down or stays the same within the
    // context)
    float mStepChances[2];

    // random reset chance (per char)
    double mResetChance;

    // Animation stuff
    quint32 mFrameId;
    QTimer mTimer;

    // Random Generation
    quint8 distributedChoice(float dist[2], quint8 size);
    quint8 makeStep(quint8 &before);

    QRandomGenerator mRandomGenerator;
};

// ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++ ++++
// ++++ Define the singleton type provider function (callback).
static QObject* themeSingletonProvider(QQmlEngine* engine, QJSEngine* scriptEngine) {
    Q_UNUSED(scriptEngine)

    TextGenerator* singletonTheme = new TextGenerator(500, 80, engine);
    return singletonTheme;
}

#endif // TEXTGENERATOR_H
