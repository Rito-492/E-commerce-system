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
    void sglSendFromServer(const QByteArray& message, const qintptr& socketDescriptor);
    void sglReceivedReomClient(const QByteArray& message, const qintptr& socketDescriptor);
    void sendDisconnectedSignal(const qintptr& socketDiscriptor);

public slots:

    void dataSendFromServer(const QByteArray& message, const qintptr& socketDescriptor);
    void dataReceivedFromClient(const QByteArray& message, const qintptr& socketDescriptor);
    void disconnectChoice(const qintptr& SocketDescriptor, const quint16& peerPort, QThread *currentThread);

private:

    QMap<qintptr, TcpSocket*>socketPool;
};

#endif // TCPSERVER_H
