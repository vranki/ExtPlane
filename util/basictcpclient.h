#ifndef BASICTCPCLIENT_H
#define BASICTCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QAbstractSocket>
#include <QTimer>

/**
 * @brief The BasicTcpClient class is a very basic text-based
 * TCP client that can read and write lines to a TCP socket.
 */
class BasicTcpClient : public QTcpSocket
{
    Q_OBJECT
    Q_PROPERTY(QString lineEnding READ lineEnding WRITE setLineEnding NOTIFY lineEndingChanged)
    Q_PROPERTY(QString peerName READ peerName NOTIFY connectionChanged)
    Q_PROPERTY(int peerPort READ peerPort NOTIFY connectionChanged)

public:
    explicit BasicTcpClient(QObject *parent = nullptr);
    virtual ~BasicTcpClient() {}
    virtual void connectTo(QString host = QString(), int port=0);
    void writeLine(QString line);
    QString lineEnding() const;

public slots:
    void setLineEnding(QString lineEnding);

signals:
    void tcpClientConnected();
    void receivedLine(QString & line);
    void connectionMessage(QString text);
    void networkError(QString errorString);
    void lineEndingChanged(QString lineEnding);
    void connectionChanged();

private slots:
    void socketConnected();
    void socketError(QAbstractSocket::SocketError err);
    void readClient();
    void tryReconnect();

private:
    QTimer reconnectTimer;
    QString m_lineEnding;
    QString m_host;
    int m_port;
};

#endif // BASICTCPCLIENT_H
