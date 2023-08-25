#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "Application.h"

int main(int argc, char *argv[])
{
    Application application(argc, argv);

    QQmlApplicationEngine engine;

    application.registerQmlTypes();
    application.registerQmlSingletones();

    const QUrl            url(u"qrc:/qml/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &application, [url](QObject *obj, const QUrl &objUrl)
    {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    engine.load(url);

    return application.exec();
}
