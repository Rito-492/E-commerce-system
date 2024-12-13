#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QThread>

class TcpSocket : public QTcpSocket
{
    Q_OBJECT

public:

    explicit TcpSocket(QTcpSocket *parent = nullptr);
    void dataReceived();
    qintptr socketDescriptor;

signals:

    //NOTE:断开连接的用户信息，此信号必须发出！线程管理类根据信号计数的
    void userDisconnected(qintptr, const quint16, QThread *);
    void sendByteArray(QByteArray, qintptr);

public slots:

    void dataSend(QByteArray, qintptr);

};

#endif // TCPSOCKET_H
