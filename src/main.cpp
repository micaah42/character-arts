#include <QDebug>
#include <QQmlContext>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "textgenerator.h"
#include "dynamicsettings.h"

void printApplicationStart();

int main(int argc, char* argv[]) {
    // qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    app.setApplicationVersion("0.0.3");
    printApplicationStart();

    QQmlApplicationEngine engine;

    // load the settings first
    DynamicSettings settings;
    engine.rootContext()->setContextProperty("settings", &settings);

    TextGenerator textGenerator(settings);
    engine.rootContext()->setContextProperty("TextGenerator", &textGenerator);

    const QUrl url(QStringLiteral("qrc:/Main.qml"));
    QObject::connect(
        &engine, &QQmlApplicationEngine::objectCreated, &app,
        [url](QObject* obj, const QUrl& objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}

void printApplicationStart()
{
    int width = 100;
    qWarning().noquote() << QString().fill('#', width);
    qWarning().noquote() << "##" + QString("").fill(' ', width - 4) + "##";
    qWarning().noquote() << QString("##    CHARACTER ARTS").leftJustified(width - 2, ' ') + "##";
    qWarning().noquote() << QString("##    ").leftJustified(width - 6, '-') + "    ##";
    qWarning().noquote() << QString("##    by: michael / therushdude").leftJustified(width - 2, ' ') + "##";
    qWarning().noquote() << QString("##    version: %1").arg(QCoreApplication::applicationVersion()).leftJustified(width - 2, ' ') + "##";
    qWarning().noquote() << "##" + QString("").fill(' ', width - 4) + "##";
    qWarning().noquote() << QString().fill('#', width);
    qWarning().noquote() << "";
}
