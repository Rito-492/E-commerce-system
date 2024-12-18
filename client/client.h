#ifndef CLIENT_H
#define CLIENT_H

#include <QDateTime>
#include <QJsonObject>
#include <QString>
#include <QVariantMap>

const QString defultAvatar = "C:/Users/Rito-492/Documents/Study/OOP/E-commerce-system/imagebase/defultAvatar.png";

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

    // 静态成员函数，将 Client 对象转换为 QJsonObject
    static QJsonObject toJsonObject(const Client& client);
    // 静态成员函数：从QJsonObject转换为Client对象
    static Client fromJsonObject(const QJsonObject& json);

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
