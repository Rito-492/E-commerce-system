#include "tcpsocket.h"

int num =0;

TcpSocket::TcpSocket(QTcpSocket *parent) : QTcpSocket(parent)
{
    qRegisterMetaType<qintptr>("qintptr");
    connect(this, &TcpSocket::readyRead, [=](){
        dataReceived();
    });
    connect(this, &TcpSocket::disconnected, [=]() {

        qDebug() << "My socket : disconnect" ;
        qDebug() << "1.socketDescriptor is " << socketDescriptor;

        //发送断开连接的用户信息
        emit userDisconnected(socketDescriptor, this->peerPort(), QThread::currentThread());
        this->deleteLater();
    });
}

void TcpSocket::dataSend(QByteArray message,qintptr socketDescriptor)
{
    qDebug() << "My socket:" << socketDescriptor << num++;
    qDebug() << message;
    qDebug() << this->socketDescriptor << "好家伙" << socketDescriptor;
    if (socketDescriptor == this->socketDescriptor)
    {
        if(message.length() > 0)
            write(message);
    }
}

void TcpSocket::dataReceived()
{

    qDebug() << "wait";
    while(bytesAvailable() > 0)
    {
        QByteArray datagram;
        datagram = readAll();
        emit sendByteArray(datagram,this->socketDescriptor);
    }

}
