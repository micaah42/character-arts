#ifndef KEYLISTENER_H
#define KEYLISTENER_H

#include <QMap>
#include <QObject>
#include <QTime>

class KeyListener : public QObject
{
    Q_OBJECT
public:
    explicit KeyListener(QObject *parent = nullptr);

signals:
    void keyPressed(Qt::Key key);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QMap<Qt::Key, QTime> _lastSignalEmit;
};

#endif // KEYLISTENER_H
