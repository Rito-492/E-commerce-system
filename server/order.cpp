#include "order.h"

Order::Order():orderId(0),orderProductNum(0),orderProductId(0),orderClientId(0),orderCheck(0),orderHide(0)
{

}

Order::Order(int orderId, QString orderProductName, int orderProductNum, QString orderProductStyle, int orderProductId, QString orderCost, QDateTime orderTime, QString orderClient, int orderClientId, int orderCheck, int orderHide)
{
    this->orderId = orderId;
    this->orderProductName = orderProductName;
    this->orderProductNum = orderProductNum;
    this->orderProductStyle = orderProductStyle;
    this->orderProductId = orderProductId;
    this->orderCost = orderCost;
    this->orderTime = orderTime;
    this->orderClient = orderClient;
    this->orderClientId = orderClientId;
    this->orderCheck = orderCheck;
    this->orderHide = orderHide;
}

int Order::getOrderHide() const
{
    return orderHide;
}

void Order::setOrderHide(int value)
{
    orderHide = value;
}

int Order::getOrderCheck() const
{
    return orderCheck;
}

void Order::setOrderCheck(int value)
{
    orderCheck = value;
}

int Order::getOrderClientId() const
{
    return orderClientId;
}

void Order::setOrderClientId(int value)
{
    orderClientId = value;
}

QString Order::getOrderClient() const
{
    return orderClient;
}

void Order::setOrderClient(const QString &value)
{
    orderClient = value;
}

QDateTime Order::getOrderTime() const
{
    return orderTime;
}

void Order::setOrderTime(const QDateTime &value)
{
    orderTime = value;
}

QString Order::getOrderCost() const
{
    return orderCost;
}

void Order::setOrderCost(const QString &value)
{
    orderCost = value;
}

int Order::getOrderProductId() const
{
    return orderProductId;
}

void Order::setOrderProductId(int value)
{
    orderProductId = value;
}

QString Order::getOrderProductStyle() const
{
    return orderProductStyle;
}

void Order::setOrderProductStyle(const QString &value)
{
    orderProductStyle = value;
}

int Order::getOrderProductNum() const
{
    return orderProductNum;
}

void Order::setOrderProductNum(int value)
{
    orderProductNum = value;
}

QString Order::getOrderProductName() const
{
    return orderProductName;
}

void Order::setOrderProductName(const QString &value)
{
    orderProductName = value;
}

int Order::getOrderId() const
{
    return orderId;
}

void Order::setOrderId(int value)
{
    orderId = value;
}
