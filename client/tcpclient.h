#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class TcpClient : public QObject {
    Q_OBJECT

public:
    TcpClient(QObject *parent = nullptr);
    void connectToServer(const QString &host, quint16 port);

private slots:
    void onConnected();
    void onReadyRead();

private:
    QTcpSocket *socket;
};

#endif // TCPCLIENT_H
