#include "client.h"

Client::Client(): clientId(0), clientBought(0)
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
