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
        qDebug() << message << Qt::endl << Qt::endl;
        //QMessageBox::warning(nullptr, "Message", message);
    }
}

void TcpSocket::receivedMessage()
{
    QByteArray datagram;
    if(bytesAvailable() > 0)
    {
        datagram.append(this->readAll());
    }
    qDebug() << "ReceivedMessage:" << datagram;
    // QMessageBox::information(nullptr, "ReceivedMessage", datagram);
    emit sendAByteArray(datagram);
}
