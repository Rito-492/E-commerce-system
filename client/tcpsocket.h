#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QMessageBox>
#include <QMutex>
#include <QObject>
#include <QTcpSocket>

class TcpSocket : public QTcpSocket
{
    Q_OBJECT

public:

    explicit TcpSocket(QObject *parent = nullptr);
    QMutex mutex;

public slots:

    void sendMessageToServer(QByteArray message);
    void receivedMessage();

signals:

    void sendAByteArray(const QByteArray datagram);
};

#endif // TCPSOCKET_H
