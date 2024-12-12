#include "tcpclient.h"

#include <QDebug>

TcpClient::TcpClient(QObject *parent) : QObject(parent), socket(new QTcpSocket(this)) {
    connect(socket, &QTcpSocket::connected, this, &TcpClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &TcpClient::onReadyRead);
}

void TcpClient::connectToServer(const QString &host, quint16 port) {
    socket->connectToHost(host, port);
}

void TcpClient::onConnected() {
    qDebug() << "Connected to server!";
    // Send a test message to the server
    socket->write("Hello, server!");
}

void TcpClient::onReadyRead() {
    QByteArray data = socket->readAll();
    qDebug() << "Received:" << data;
}
