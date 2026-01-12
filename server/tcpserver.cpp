#include "tcpserver.h"

TcpServer::TcpServer(QObject *parent, int port) : QTcpServer(parent)
{
    listen(QHostAddress::LocalHost, port);
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    TcpSocket *socket = new TcpSocket;

    qDebug() << "A new connection !";

    socket->setSocketDescriptor(socketDescriptor);
    // qDebug() << "socketDescriptor: " << socket->socketDescriptor;
    // socket->socketDescriptor = socketDescriptor;

    connect(socket, SIGNAL(sendByteArray(QByteArray,qintptr)), this, SLOT(dataReceivedFromClient(QByteArray,qintptr)));
    connect(socket, SIGNAL(userDisconnected(qintptr, const quint16, QThread *)), this, SLOT(disconnectChoice(qintptr, const quint16, QThread *)));
    connect(this, SIGNAL(sglSendFromServer(QByteArray, qintptr)), socket, SLOT(dataSend(QByteArray, qintptr)));

    auto thread = ThreadHandle::getClass().getThread();
    socket->moveToThread(thread);
    socketPool.insert(socketDescriptor, socket);
}

void TcpServer::dataSendFromServer(const QByteArray& message, const qintptr& socketDiscriptor)
{
    qDebug() << "信号传递" << message;
    emit sglSendFromServer(message, socketDiscriptor);
}

void TcpServer::dataReceivedFromClient(const QByteArray& message, const qintptr& socketDiscriptor)
{
    qDebug() << "Received from client:" << message;
    emit sglReceivedReomClient(message, socketDiscriptor);
}

void TcpServer::disconnectChoice(const qintptr& SocketDescriptor, const quint16& peerPort, QThread *currentThread)
{
    Q_UNUSED(peerPort);

    TcpSocket *socket = socketPool[SocketDescriptor];
    socketPool.remove(SocketDescriptor);
    if (socket)
        socket->close();
    if (socket)
        socket->deleteLater();
    ThreadHandle::getClass().removeThread(currentThread);
    emit sendDisconnectedSignal(SocketDescriptor);
}



