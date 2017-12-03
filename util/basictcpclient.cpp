#include "basictcpclient.h"
#include "../util/console.h"

BasicTcpClient::BasicTcpClient(QObject *parent) :  QTcpSocket(parent), _port(0), m_lineEnding("\n")
{
    connect(this, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(&reconnectTimer, SIGNAL(timeout()), this, SLOT(tryReconnect()));
}


void BasicTcpClient::connectTo(QString host, unsigned int port) {
    close();
    _host = host;
    _port = port;
    abort();
    tryReconnect();
}

void BasicTcpClient::writeLine(QString line)
{
    write(QString(line + m_lineEnding).toUtf8());
    flush();
    INFO << "wrote" << line;
}

QString BasicTcpClient::lineEnding() const
{
    return m_lineEnding;
}

void BasicTcpClient::setLineEnding(QString lineEnding)
{
    if (m_lineEnding == lineEnding)
        return;

    m_lineEnding = lineEnding;
    emit lineEndingChanged(lineEnding);
}


void BasicTcpClient::tryReconnect() {
    emit connectionMessage(QString("Connecting to %1:%2..").arg(_host).arg(_port));

    reconnectTimer.stop();
    connectToHost(_host, _port);
}

void BasicTcpClient::socketConnected() {
    reconnectTimer.stop();
    emit tcpClientConnected();
}


void BasicTcpClient::socketError(QAbstractSocket::SocketError err) {
    INFO << "Socket error:" << errorString();
    emit networkError(errorString() + " : " + _host + ":" + QString::number(_port));
    emit connectionMessage(errorString() + " : " + peerName() + ":" + QString::number(peerPort()));
    reconnectTimer.setInterval(5000);
    reconnectTimer.start();
}

void BasicTcpClient::readClient() {
    while(canReadLine()) {
        QByteArray lineBA = readLine();
        QString line = QString(lineBA).trimmed();
        emit receivedLine(line);
    }
}
