#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TcpServer : public QObject {
    Q_OBJECT

public:
    TcpServer(QObject *parent = nullptr);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    QTcpServer *server;
    QList<QTcpSocket*> clients;
};

#endif // TCPSERVER_H
