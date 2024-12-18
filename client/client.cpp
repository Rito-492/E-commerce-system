#include "client.h"

Client::Client(): clientId(0), clientBought(0), clientImage(defultAvatar)
{

}

Client::Client(QString clientName, int clientId, QString clientPhone, QDateTime clientSignTime, QString clientPwd, int clientBought, QString clientImage)
{
    this->clientName = clientName;
    this->clientId = clientId;
    this->clientPhone = clientPhone;
    this->clientSignTime = clientSignTime;
    this->clientPwd = clientPwd;
    this->clientBought = clientBought;
    this->clientImage = clientImage;
}

QJsonObject Client::toJsonObject(const Client& client) {
    QJsonObject json;
    json["client_name"] = client.clientName;
    json["client_id"] = client.clientId;
    json["client_phone"] = client.clientPhone;
    json["client_sign_time"] = client.clientSignTime.toString("yyyy-MM-dd");
    json["client_pwd"] = client.clientPwd;
    json["client_bought"] = client.clientBought;
    json["client_image"] = client.clientImage;
    return json;
}

Client Client::fromJsonObject(const QJsonObject& json) {
    Client client;
    client.clientName = json["client_name"].toString();
    client.clientId = json["client_id"].toInt();
    client.clientPhone = json["client_phone"].toString();
    client.clientSignTime = QDateTime::fromString(json["client_sign_time"].toString(), "yyyy-MM-dd");
    client.clientPwd = json["client_pwd"].toString();
    client.clientBought = json["client_bought"].toInt();
    client.clientImage = json["client_image"].toString();
    return client;
}

QString Client::getClientName() const
{
    return clientName;
}

int Client::getClientId() const
{
    return clientId;
}

QString Client::getClientPhone() const
{
    return clientPhone;
}

QDateTime Client::getClientSignTime() const
{
    return clientSignTime;
}

QString Client::getClientPwd() const
{
    return clientPwd;
}

int Client::getClientBought() const
{
    return clientBought;
}

QString Client::getClientImage() const
{
    return clientImage;
}

void Client::setClientName(QString clientName)
{
    this->clientName = clientName;
}

void Client::setClientId(int clientId)
{
    this->clientId = clientId;
}

void Client::setClientPhone(QString clientPhone)
{
    this->clientPhone = clientPhone;
}

void Client::setClientSignTime(QDateTime clientSignTime)
{
    this->clientSignTime = clientSignTime;
}

void Client::setClientPwd(QString clientPwd)
{
    this->clientPwd = clientPwd;
}

void Client::setClientBought(int clientBought)
{
    this->clientBought = clientBought;
}

void Client::setClientImage(QString clientImage)
{
    this->clientImage = clientImage;
}
