#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include "extplanetransformer.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    //qmlRegisterType<TcpServer*>("org.extplane", 1, 0, "TcpServer");
    ExtplaneTransformer transformer;
    engine.rootContext()->setContextProperty("transformer", &transformer);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
