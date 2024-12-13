#include "tcpsocket.h"

TcpSocket::TcpSocket(QObject *parent) : QTcpSocket(parent)
{
    qRegisterMetaType<qintptr>("qintptr");
    connect(this, &TcpSocket::readyRead, [=](){
        receivedMessage();
    });
    connect(this, &QTcpSocket::disconnected, [=]() {
        QMessageBox::information(nullptr, "Title", "disconnected");
        this->deleteLater();
    });
}

void TcpSocket::sendMessageToServer(QByteArray message)
{
    QMutexLocker locker(&mutex);
    if(message.length() > 0)
    {
        write(message);
        qDebug() << "发送成功";
        QMessageBox::information(nullptr, "Title", "发送成功");
    }
}

void TcpSocket::receivedMessage()
{

    qDebug() << "Myreceive";
    QByteArray datagram;
    if(bytesAvailable() > 0)
    {
        datagram.append(this->readAll());
    }
    qDebug() << datagram;
    emit sendAByteArray(datagram);
}
