#include <QQmlContext>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QTimer>

#include "../src/dynamicsettings.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    DynamicSettings settings;
    engine.rootContext()->setContextProperty("settings", &settings);

    settings.createNewSetting("a.1", QVariant(2112));
    settings.createNewSetting("a.2", QVariant(21.12));

    // b1 will be changed
    settings.createNewSetting("b.1", QVariant("2112"));
    QVariant b1;
    settings.subscribeSetting("b.1", b1);

    QTimer b1Changer;
    QObject::connect(&b1Changer, &QTimer::timeout, [&settings, &b1](){
        settings.changeSetting("b.1", b1.toString() + "_");
    });
    b1Changer.setInterval(5000);
    // b1Changer.start();

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
