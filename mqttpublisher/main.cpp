#include "mqttpublisher.h"
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <csignal>

void sigHandler(int s)
{
    std::signal(s, SIG_DFL);
    qApp->quit();
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    std::signal(SIGINT,  sigHandler);
    std::signal(SIGTERM, sigHandler);
    QTimer pollTimer;
    pollTimer.setInterval(16);
    pollTimer.setSingleShot(false);

    MQTTPublisher publisher(nullptr);
    QTimer::connect(&pollTimer, &QTimer::timeout, &publisher, &MQTTPublisher::poll);
    publisher.start();
    pollTimer.start();
    auto ret = app.exec();
    publisher.stop();
    qDebug() << "Graceful exit, thanks!";
    return ret;
}
