#include "tcpserver.h"

#include <QDebug>

TcpServer::TcpServer(QObject *parent) : QObject(parent), server(new QTcpServer(this)) {
    connect(server, &QTcpServer::newConnection, this, &TcpServer::onNewConnection);
    if (!server->listen(QHostAddress::Any, 8284)) {
        qDebug() << "Server could not start!";
    } else {
        qDebug() << "Server started!";
    }
}

void TcpServer::onNewConnection() {
    QTcpSocket *clientSocket = server->nextPendingConnection();
    clients.append(clientSocket);
    connect(clientSocket, &QTcpSocket::readyRead, this, &TcpServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, [this, clientSocket]() {
        clients.removeAll(clientSocket);
        clientSocket->deleteLater();
    });
}

void TcpServer::onReadyRead() {
    QTcpSocket *clientSocket = qobject_cast<QTcpSocket*>(sender());
    if (clientSocket) {
        QByteArray data = clientSocket->readAll();
        qDebug() << "Received:" << data;
        // Echo the data back to the client
        clientSocket->write(data);
    }
}
