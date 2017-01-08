#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "../extplane-server/tcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);

    TcpServer tcpServer(0, 0);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("tcpserver", &tcpServer);
    engine.load(QUrl(QLatin1String("qrc:/main.qml")));

    return app.exec();
}
