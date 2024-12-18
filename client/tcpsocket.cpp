#include "tcpsocket.h"

TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent)
{
    qRegisterMetaType<qintptr>("qintptr");
    connect(this, &TcpSocket::readyRead, [=](){
        receivedMessage();
    });
    connect(this, &QTcpSocket::disconnected, [=]() {
        // QMessageBox::information(nullptr, "Title", "disconnected");
        this->deleteLater();
    });
}

void TcpSocket::sendMessageToServer(QByteArray message)
{
    QMutexLocker locker(&mutex);
    if(message.length() > 0)
    {
        write(message);
        // QMessageBox::information(nullptr, "Title", "发送成功");
    }
}

void TcpSocket::receivedMessage()
{
    // qDebug() << ;
    QByteArray datagram;
    if(bytesAvailable() > 0)
    {
        datagram.append(this->readAll());
    }
    QMessageBox::information(nullptr, "ReceivedMessage", datagram);
    emit sendAByteArray(datagram);
}
