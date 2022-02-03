#ifndef APPLICATIONSETTINGS_H
#define APPLICATIONSETTINGS_H

#include <QObject>
#include <QVariantList>

class ApplicationSettings : public QObject
{
    Q_OBJECT

    // settings
    Q_PROPERTY(int fps READ fps WRITE setFps NOTIFY fpsChanged)
    Q_PROPERTY(int cols READ cols WRITE setCols NOTIFY colsChanged)
    Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY rowsChanged)
    Q_PROPERTY(double fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(int resetChance READ resetChance WRITE setResetChance NOTIFY resetChanceChanged)
    Q_PROPERTY(QVariantList stepChances READ stepChances WRITE setStepChances NOTIFY stepChancesChanged)
    Q_PROPERTY(QString charContextAsString READ charContextAsString WRITE setCharContextAsString NOTIFY charContextChanged)


public:
    static ApplicationSettings* I();
    void init();

    // load a preset
    void loadFromFile();

    // SETTINGS
    double fps() const;
    void setFps(double fps);

    int cols() const;
    void setCols(const int cols);

    int rows() const;
    void setRows(const int rows);

    QVariantList stepChances();
    float stepChance(int of) const;
    void setStepChances(QVariantList stepChances);

    int resetChance() const;
    void setResetChance(const int resetChance);

    const QList<QChar>& charContext() const;
    QString charContextAsString() const;
    void setCharContext(const QList<QChar> &charContext);
    void setCharContextAsString(const QString& chars);

    double fontSize() const;
    void setFontSize(double fontSize);


signals:
    void stepChancesChanged();
    void fontSizeChanged();
    void measuredFpsChanged();
    void fpsChanged();
    void colsChanged();
    void rowsChanged();
    void resetChanceChanged();
    void charContextChanged();


private:
    explicit ApplicationSettings(QObject* parent = nullptr);
    static ApplicationSettings* instance;

    // how many chars in the matrix are set to max for each frame
    int mNumResets;

    // image char-dimensions
    int mCols, mRows;

    // and font size
    double mFontSize;

    // redraw rate
    double mFps;

    // interpretation of chars - basically a list that puts similar chars together
    QList<QChar> mCharContext;

    // step chances (wether the char goes up, down or stays the same within the
    // context)
    float mStepChances[2];
};


#endif // APPLICATIONSETTINGS_H
