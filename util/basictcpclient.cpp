#include "basictcpclient.h"
#include "../util/console.h"

BasicTcpClient::BasicTcpClient(QObject *parent) : QTcpSocket(parent)
  , m_lineEnding("\n")
  , m_port(0)
{
    connect(this, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError(QAbstractSocket::SocketError)));
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(&reconnectTimer, SIGNAL(timeout()), this, SLOT(tryReconnect()));
    reconnectTimer.setSingleShot(false);
}


void BasicTcpClient::connectTo(QString host, int port) {
    close();
    reconnectTimer.setInterval(5000);
    reconnectTimer.start();
    if(host.isEmpty() || !port) {
        INFO << "host or port not set - can't connect" << host << port;
        return;
    }
    m_host = host;
    m_port = port;
    emit connectionChanged();
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
    if(m_host.isEmpty() || !m_port) {
        emit connectionMessage("Please set host and port to connect");
        return;
    } else {
        emit connectionMessage(QString("Connecting to %1:%2..").arg(m_host).arg(m_port));
        connectToHost(m_host, static_cast<quint16>(m_port));
    }
}

void BasicTcpClient::socketConnected() {
    reconnectTimer.stop();
    emit tcpClientConnected();
}


void BasicTcpClient::socketError(QAbstractSocket::SocketError err) {
    Q_UNUSED(err);
    INFO << "Socket error:" << errorString();
    emit networkError(errorString() + " : " + m_host + ":" + QString::number(m_port));
    emit connectionMessage(errorString() + " : " + peerName() + ":" + QString::number(peerPort()));
}

void BasicTcpClient::readClient() {
    while(canReadLine()) {
        QByteArray lineBA = readLine();
        QString line = QString(lineBA).trimmed();
        emit receivedLine(line);
    }
}
