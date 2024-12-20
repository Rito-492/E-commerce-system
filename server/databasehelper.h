#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

#include "client.h"
#include "order.h"
#include "product.h"

#include <QDebug>
#include <QMessageBox>
#include <QObject>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlField>
#include <QSqlIndex>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QVariant>
#include <QVariantList>

class DatabaseHelper : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseHelper(QObject *parent = nullptr);
    ~DatabaseHelper();

    bool connectToDatabase(const QString &hostName, const int &port, const QString &databaseName,
                           const QString &userName, const QString &password);
    bool executeQuery(const QString &query);
    bool insertRecord(const QString &tableName, const QVariantMap &record);
    bool updateRecord(const QString &tableName, const QVariantMap &setFields,
                      const QVariantMap &whereFields);
    bool deleteRecord(const QString &tableName, const QVariantMap &whereFields);
    QList<QVariantMap> selectRecords(const QString &tableName, const QVariantMap &whereFields = QVariantMap());
    QList<QVariantMap> getAllRecords(const QString &tableName);

    bool isValidTableName(const QString &tableName);

    QSqlDatabase& getDatabase();

public slots:

    // Client表的增删改查
    QList<Client> getClientList();

    Client getClientByNameAndPwd(const QString& name, const QString& pwd);

    bool insertClient(const Client& client);

    bool deleteClientByNameAndPwd(const QString& name, const QString& pwd);

    bool updateClient(const Client& client);

    QList<Client> getClientLikeList(const QString& mess);

    // Order表的增删改查
    QList<Order> getOrderList();

    QList<Order> getOrderListByInfo(const Order& order);

    bool addOrder(const Order& order);

    QList<Order> getOrderLikeList(const QString& mess);

    QList<Order> getOrderListHistory();

    bool updateHistory(const Order& order);

    bool deleteShoppingCart(const Order& order);

    // 商品表的增删改查
    QList<Product>  getProductList(const int& choose);

    QList<Product>  getProductListByInfo(const Product& product);

    bool addProduct(const Product& product);

    bool deleteProductByInfo(const Product& product);

    bool updateProductByInfo(const Product& product);

    QList<Product> getProductLikeList(const QString& mess);

private:
    static QString databaseDriver;
    QSqlDatabase db;
    QSqlQuery result;

    QList<Client> ClientList;
    QList<Product> ProductList;
    QList<Order> OrderList;

    QList<QVariantMap> allRecords;

};

#endif // DATABASEHELPER_H
