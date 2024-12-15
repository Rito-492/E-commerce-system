#ifndef CLIENT_H
#define CLIENT_H

#include <QDateTime>
#include <QString>

class Client
{
private:
    QString clientName;
    int clientId;
    QString clientPhone;
    QDateTime clientSignTime;
    QString clientPwd;
    int clientBought;
    QString clientImage;

public:
    Client();
    Client(QString clientName, int clientId, QString clientPhone, QDateTime clientSignTime, QString clientPwd, int clientBought, QString clientImage);

    void setClientName(QString clientName);
    void setClientId(int clientId);
    void setClientPhone(QString clientPhone);
    void setClientSignTime(QDateTime clientSignTime);
    void setClientPwd(QString clientPwd);
    void setClientBought(int clientBought);
    void setClientImage(QString clientImage);

    QString getClientName() const;
    int getClientId() const;
    QString getClientPhone() const;
    QDateTime getClientSignTime() const;
    QString getClientPwd() const;
    int getClientBought() const;
    QString getClientImage() const;
};

#endif // CLIENT_H
