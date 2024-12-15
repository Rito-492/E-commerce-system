#ifndef ORDER_H
#define ORDER_H

#include <QDateTime>
#include <QString>

class Order
{
public:
    Order();
    Order(int orderId, QString orderProductName, int orderProductNum, QString orderProductStyle, int orderProductId, QString orderCost, QDateTime orderTime, QString orderClient, int orderClientId, int orderCheck, int orderHide);


    int getOrderHide() const;
    void setOrderHide(int value);

    int getOrderCheck() const;
    void setOrderCheck(int value);

    int getOrderClientId() const;
    void setOrderClientId(int value);

    QString getOrderClient() const;
    void setOrderClient(const QString &value);

    QDateTime getOrderTime() const;
    void setOrderTime(const QDateTime &value);

    QString getOrderCost() const;
    void setOrderCost(const QString &value);

    int getOrderProductId() const;
    void setOrderProductId(int value);

    QString getOrderProductStyle() const;
    void setOrderProductStyle(const QString &value);

    int getOrderProductNum() const;
    void setOrderProductNum(int value);

    QString getOrderProductName() const;
    void setOrderProductName(const QString &value);

    int getOrderId() const;
    void setOrderId(int value);

private:
    int orderId;
    QString orderProductName;
    int orderProductNum;
    QString orderProductStyle;
    int orderProductId;
    QString orderCost;
    QDateTime orderTime;
    QString orderClient;
    int orderClientId;
    int orderCheck;
    int orderHide;
};

#endif // ORDER_H
