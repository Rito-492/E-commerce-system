#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcpsocket.h"
#include "threadhandle.h"

#include <QList>
#include <QMap>
#include <QObject>
#include <QTcpServer>

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:

    explicit TcpServer(QObject *parent = nullptr,int port = 0);
    void incomingConnection(qintptr handle);

signals:
    void addChatRoom(qintptr socketDescriptor);
    void sglSendFromServer(QByteArray message, qintptr socketDescriptor);
    void sglReceivedReomClient(QByteArray message, qintptr socketDescriptor);
    void sendDisconnectedSignal(qintptr socketDiscriptor);

public slots:

    void dataSendFromServer(QByteArray message, qintptr socketDescriptor);
    void dataReceivedFromClient(QByteArray message, qintptr socketDescriptor);
    void disconnectChoice(qintptr SocketDescriptor, const quint16 peerPort, QThread *currentThread);

private:

    QMap<qintptr, TcpSocket*>socketPool;
};

#endif // TCPSERVER_H
