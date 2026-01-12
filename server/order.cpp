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

QJsonObject Order::toJsonObject(const Order& order) {
    QJsonObject json;
    json["order_id"] = order.orderId;
    json["order_product_name"] = order.orderProductName;
    json["order_product_num"] = order.orderProductNum;
    json["order_product_style"] = order.orderProductStyle;
    json["order_product_id"] = order.orderProductId;
    json["order_cost"] = order.orderCost; // 假设 order_cost 是 qreal 类型，并格式化为两位小数
    json["order_time"] = order.orderTime.toString("yyyy-MM-dd");
    json["order_client"] = order.orderClient;
    json["order_client_id"] = order.orderClientId;
    json["order_check"] = order.orderCheck;
    json["order_hide"] = order.orderHide;
    return json;
}

Order Order::fromJsonObject(const QJsonObject& json) {
    Order order;
    order.orderId = json["order_id"].toInt();
    order.orderProductName = json["order_product_name"].toString();
    order.orderProductNum = json["order_product_num"].toInt();
    order.orderProductStyle = json["order_product_style"].toString();
    order.orderProductId = json["order_product_id"].toInt();
    order.orderCost = json["order_cost"].toString();
    order.orderTime = QDateTime::fromString(json["order_time"].toString(), "yyyy-MM-dd");
    order.orderClient = json["order_client"].toString();
    order.orderClientId = json["order_client_id"].toInt();
    order.orderCheck = json["order_check"].toInt();
    order.orderHide = json["order_hide"].toInt();
    return order;
}

QJsonObject Order::toJsonObjectArray(const QList<Order>& orders) {

    QJsonObject jsonObject;
    QJsonArray jsonArray;

    for (const Order& order : orders) {
        jsonArray.append(Order::toJsonObject(order));
    }

    jsonObject["orders"] = jsonArray;
    return jsonObject;

}

QList<Order> Order::fromJsonObjectArray(const QJsonObject& jsonObj) {

    QList<Order> orders;
    QJsonArray jsonArray = jsonObj.value("orders").toArray();

    for (const QJsonValue& value : jsonArray) {
        if (value.isObject()) { // 检查值是否是一个对象
            QJsonObject json = value.toObject(); // 现在可以安全地转换为 QJsonObject
            orders.append(Order::fromJsonObject(json));
        } else {
            // 处理错误或者忽略非对象类型的值
            qWarning() << "Expected a QJsonObject in the JSON array, but got something else.";
        }
    }

    return orders;
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
