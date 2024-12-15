#include "databasehelper.h"

QString DatabaseHelper::databaseDriver = "QODBC";

DatabaseHelper::DatabaseHelper(QObject *parent)
    : QObject(parent), db(QSqlDatabase::addDatabase(databaseDriver))
{
}

DatabaseHelper::~DatabaseHelper()
{
    if (db.isOpen())
        db.close();
}

bool DatabaseHelper::connectToDatabase(const QString &hostName, const int &port, const QString &databaseName,
                                       const QString &userName, const QString &password)
{
    db.setHostName(hostName);
    db.setPort(port);
    db.setDatabaseName(databaseName);
    db.setUserName(userName);
    db.setPassword(password);

    if (!db.open()) {
        // qDebug() << "Error: Unable to establish a database connection.";
        // qDebug() << db.lastError().text();
        // 可以选择在这里显示一个错误消息框给用户
        QMessageBox::critical(nullptr, QObject::tr("Database Connection Error"),
                              db.lastError().text(), QMessageBox::Cancel);
        return false;
    }

    qDebug() << "Database connection established.";
    return true;
}

bool DatabaseHelper::executeQuery(const QString &query)
{
    QSqlQuery sqlQuery(db);

    if (!sqlQuery.exec(query)) {
        qDebug() << "Error executing query:" << sqlQuery.lastError().text();
        return false;
    }
    return true;
}

// INSERT INTO 表名([字段1,字段2..])VALUES('值1','值2'..),[('值1','值2'..)..];
bool DatabaseHelper::insertRecord(const QString &tableName, const QVariantMap &record)
{
    QString fields, values;
    QVariantList placeholders;

    // 验证表名不为空
    if (!isValidTableName(tableName)) {
        qWarning() << "Invalid table name format or the tablename is empty!";
        return false;
    }

    // 构建字段和占位符列表
    for (auto it = record.begin(); it != record.end(); ++it) {
        if (!fields.isEmpty()) fields += ", ";
        if (!values.isEmpty()) values += ", ";
        fields += it.key();
        placeholders.append("?");
        values += QString("?");
    }

    // 构建SQL插入语句
    QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName, fields, values.replace("?", QStringList(placeholders.size(), "?").join(", ")));
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare(sql);

    // 绑定值
    for (int i = 0; i < record.size(); ++i) {
        sqlQuery.addBindValue(record.value(record.keys().at(i)));
    }

    // 执行插入操作
    return sqlQuery.exec();
}

// UPDATE 表名 SET 字段1=值1,[字段2=值2...] WHERE 条件[];
bool DatabaseHelper::updateRecord(const QString &tableName, const QVariantMap &setFields, const QVariantMap &whereFields)
{
    QString setClause, whereClause;
    QList<QVariant> bindValues;

    // 验证表名不为空
    if (!isValidTableName(tableName)) {
        qWarning() << "Invalid table name format or the tablename is empty!";
        return false;
    }

    // 构建SET子句
    for (auto it = setFields.begin(); it != setFields.end(); ++it) {
        if (!setClause.isEmpty()) setClause += ", ";
        setClause += it.key() + " = ?";
        bindValues.append(it.value());
    }

    // 构建WHERE子句
    for (auto it = whereFields.begin(); it != whereFields.end(); ++it) {
        if (!whereClause.isEmpty()) whereClause += " AND ";
        whereClause += it.key() + " = ?";
        bindValues.append(it.value());
    }

    // 构建SQL更新语句
    QString sql = QString("UPDATE %1 SET %2 WHERE %3").arg(tableName, setClause, whereClause);
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare(sql);

    // 绑定值
    for (const QVariant &value : bindValues) {
        sqlQuery.addBindValue(value);
    }

    // 执行更新操作
    return sqlQuery.exec();
}

// DELETE FROM 表名 [WHERE 条件]
bool DatabaseHelper::deleteRecord(const QString &tableName, const QVariantMap &whereFields)
{
    QString whereClause;
    QList<QVariant> bindValues;

    // 验证表名不为空
    if (!isValidTableName(tableName)) {
        qWarning() << "Invalid table name format or the tablename is empty!";
        return false;
    }

    // 构建WHERE子句
    for (auto it = whereFields.begin(); it != whereFields.end(); ++it) {
        if (!whereClause.isEmpty()) whereClause += " AND ";
        whereClause += it.key() + " = ?";
        bindValues.append(it.value());
    }

    // 构建SQL删除语句
    QString sql = QString("DELETE FROM %1 WHERE %2").arg(tableName, whereClause);
    QSqlQuery sqlQuery(db);
    sqlQuery.prepare(sql);

    // 绑定值
    for (const QVariant &value : bindValues) {
        sqlQuery.addBindValue(value);
    }

    // 执行删除操作
    return sqlQuery.exec();
}

// SELECT 查询列表 FROM 表名;
QList<QVariantMap> DatabaseHelper::selectRecords(const QString &tableName, const QVariantMap &whereFields)
{
    QList<QVariantMap> result;
    QString whereClause;
    QList<QVariant> bindValues;

    // 验证表名不为空
    if (!isValidTableName(tableName)) {
        qWarning() << "Invalid table name format or the tablename is empty!";
        return result;
    }

    // 构建WHERE子句（可选）
    if (!whereFields.isEmpty()) {
        for (auto it = whereFields.begin(); it != whereFields.end(); ++it) {
            if (!whereClause.isEmpty()) whereClause += " AND ";
            whereClause += it.key() + "=?";
            bindValues.append(it.value());
        }
    }

    // 构建SQL选择语句
    QString sql = QString("SELECT * FROM %1").arg(tableName);
    if (!whereClause.isEmpty()) {
        sql += QString(" WHERE %1").arg(whereClause);
    }

    QSqlQuery sqlQuery(db);
    sqlQuery.prepare(sql);

    // 绑定值（如果有的话）
    for (const QVariant &value : bindValues) {
        sqlQuery.addBindValue(value);
    }

    // 执行选择操作并处理结果
    if (sqlQuery.exec()) {
        while (sqlQuery.next()) {
            QVariantMap row;
            QSqlRecord record = sqlQuery.record();
            int columnCount = record.count();
            for (int i = 0; i < columnCount; ++i) {
                row[record.fieldName(i)] = sqlQuery.value(i);
            }
            result.append(row);
        }
    } else {
        qWarning() << "Database error:" << sqlQuery.lastError().text()
        << "\nQuery:" << sqlQuery.lastQuery();
    }

    return result;
}

bool DatabaseHelper::isValidTableName(const QString &tableName)
{
    // 验证表名不为空
    if (tableName.isEmpty()) {
        qWarning() << "Table name cannot be empty";
        return false;
    }

    // 验证表名防止SQL注入（只允许字母、数字和下划线）
    QRegularExpression validTableName("^[a-zA-Z0-9_]+$");
    QRegularExpressionMatch match = validTableName.match(tableName);
    if (!match.hasMatch()) {
        qWarning() << "Invalid table name format";
        return false;
    }

    return true;
}

QList<QVariantMap> DatabaseHelper::getAllRecords(const QString &tableName) {

    allRecords.clear();

    // 验证表名不为空
    if (!isValidTableName(tableName)) {
        qWarning() << "Invalid table name format or the tablename is empty!";
        return allRecords;
    }

    // 构建SQL选择语句
    QString sql = QString("SELECT * FROM %1").arg(tableName);

    QSqlQuery sqlQuery(db);

    // 执行选择操作并处理结果
    if (sqlQuery.exec(sql)) {
        while (sqlQuery.next()) {
            QVariantMap row;
            QSqlRecord record = sqlQuery.record();
            int columnCount = record.count();
            for (int i = 0; i < columnCount; ++i) {
                row[record.fieldName(i)] = sqlQuery.value(i);
            }
            allRecords.append(row);
        }
    } else {
        qWarning() << "Database error:" << sqlQuery.lastError().text()
        << "\nQuery:" << sqlQuery.lastQuery();
    }

    return allRecords;
}

QList<Client> DatabaseHelper::getClientList()
{
    ClientList.clear();
    getAllRecords("`client`");

    for (const QVariantMap &tmp : allRecords) {
        Client row;
        row.setClientName(tmp.value("client_name").toString());
        row.setClientId(tmp.value("client_id").toInt());
        row.setClientPhone(tmp.value("client_phone").toString());
        row.setClientSignTime(tmp.value("client_sign_time").toDateTime());
        row.setClientPwd(tmp.value("client_pwd").toString());
        row.setClientBought(tmp.value("client_bought").toInt());
        row.setClientImage(tmp.value("client_image").toString());
        ClientList.append(row);
    }

    return ClientList;
}

Client DatabaseHelper::getClientByNameAndPwd(const QString& name, const QString& pwd)
{
    // 创建一个 QVariantMap 来存储 WHERE 子句的条件
    QVariantMap whereFields;
    whereFields["client_name"] = name;
    whereFields["client_pwd"] = pwd;

    // 调用 selectRecords 函数并获取结果
    QList<QVariantMap> tmpList = selectRecords("`client`", whereFields);

    Client row;

    if (tmpList.size() > 1) {
        qDebug() << "There is a wrong !";
    } else {
        for (const QVariantMap &tmp : allRecords) {
            row.setClientName(tmp.value("client_name").toString());
            row.setClientId(tmp.value("client_id").toInt());
            row.setClientPhone(tmp.value("client_phone").toString());
            row.setClientSignTime(tmp.value("client_sign_time").toDateTime());
            row.setClientPwd(tmp.value("client_pwd").toString());
            row.setClientBought(tmp.value("client_bought").toInt());
            row.setClientImage(tmp.value("client_image").toString());
        }
    }

    return row;
}

bool DatabaseHelper::insertClient(const Client& client)
{
    QVariantMap record;
    record["client_name"] = client.getClientName();
    record["client_id"] = client.getClientId();
    record["client_phone"] = client.getClientPhone();
    record["client_sign_time"] = client.getClientSignTime();
    record["client_pwd"] = client.getClientPwd();
    record["client_bought"] = client.getClientBought();
    record["client_image"] = client.getClientImage();

    return insertRecord("`client`", record);
}

bool DatabaseHelper::deleteClientByNameAndPwd(const QString& name, const QString& pwd)
{
    // 创建一个 QVariantMap 来存储 WHERE 子句的条件
    QVariantMap whereFields;
    whereFields["client_name"] = name;
    whereFields["client_pwd"] = pwd;

    return deleteRecord("`client`", whereFields);
}

bool DatabaseHelper::updateClient(const Client& client)
{
    QVariantMap whereFields;
    whereFields["client_id"] = client.getClientId();

    QVariantMap record;
    record["client_name"] = client.getClientName();
    record["client_id"] = client.getClientId();
    record["client_phone"] = client.getClientPhone();
    record["client_sign_time"] = client.getClientSignTime();
    record["client_pwd"] = client.getClientPwd();
    record["client_bought"] = client.getClientBought();
    record["client_image"] = client.getClientImage();

    return updateRecord("`client`", record, whereFields);
}

// QList<Client *> DatabaseHelper::getClientLikeList(QString mess)
// {
//     ClientList.clear();
//     struct soap select_soap;
//     soap_init(&select_soap);
//     soap_set_mode(&select_soap,SOAP_C_UTFSTRING);
//     lkf2__getClientLikeList res;
//     lkf2__getClientLikeListResponse rep;
//     res.arg0 = (StringTrans::QString2string(mess));
//     int result = soap_call___lkf1__getClientLikeList(&select_soap,NULL,NULL,&res,rep);
//     if(!result)
//     {
//         //        qDebug()<<"查询成功";
//         std::vector<lkf2__client*> clientList = rep.return_;
//         for(int i=0 ;i < (int)clientList.size();i++ )
//         {
//             Client* client = transObjects::transClient(clientList[i]);
//             ClientList.append(client);
//             //            qDebug()<<client->getClientName();
//         }
//     }
//     return ClientList;
// }


QList<Order> DatabaseHelper::getOrderList()
{
    OrderList.clear();
    getAllRecords("`order`");

    for (const QVariantMap &tmp : allRecords) {
        Order row;
        row.setOrderId(tmp.value("order_id").toInt());
        row.setOrderProductName(tmp.value("order_product_name").toString());
        row.setOrderProductNum(tmp.value("order_product_num").toInt());
        row.setOrderProductStyle(tmp.value("order_product_style").toString());
        row.setOrderProductId(tmp.value("order_product_id").toInt());
        row.setOrderCost(tmp.value("order_cost").toString());
        row.setOrderTime(tmp.value("order_time").toDateTime());
        row.setOrderClient(tmp.value("order_client").toString());
        row.setOrderClientId(tmp.value("order_client_id").toInt());
        row.setOrderCheck(tmp.value("order_check").toInt());
        row.setOrderHide(tmp.value("order_hide").toInt());
        OrderList.append(row);
    }

    return OrderList;
}

// QList<Order> DatabaseHelper::getOrderListByInfo(const Order& order)
// {
//     OrderList.clear();
//     struct soap select_soap;
//     soap_init(&select_soap);
//     soap_set_mode(&select_soap,SOAP_C_UTFSTRING);
//     lkf2__getOrderListByInfo res;
//     lkf2__getOrderListByInfoResponse rep;
//     res.arg0  = transObjects::retransOrder(order);
//     int result = soap_call___lkf1__getOrderListByInfo(&select_soap,NULL,NULL,&res,rep);
//     //    qDebug()<<result;
//     if(!result)
//     {
//         std::vector<lkf2__order*> orderList = rep.return_;
//         //        qDebug()<<orderList.size();
//         for(int i=0;i<(int)orderList.size();i++)
//         {
//             Order* order = transObjects::transOrder(orderList[i]);
//             OrderList.append(order);
//         }
//         qDebug() << OrderList.size();
//         for(int i=0;i<OrderList.size();i++)
//         {
//             //            qDebug() << OrderList[i]->getOrderProductName();
//         }
//     }
//     return OrderList;
// }

bool DatabaseHelper::addOrder(const Order& order)
{
    QVariantMap record;
    record["order_id"] = order.getOrderId();
    record["order_product_name"] = order.getOrderProductName();
    record["order_product_num"] = order.getOrderProductNum();
    record["order_product_style"] = order.getOrderProductStyle();
    record["order_product_id"] = order.getOrderProductId();
    record["order_cost"] = order.getOrderCost();
    record["order_time"] = order.getOrderTime();
    record["order_client"] = order.getOrderClient();
    record["order_client_id"] = order.getOrderClientId();
    record["order_check"] = order.getOrderCheck();
    record["order_hide"] = order.getOrderHide();

    return insertRecord("`order`", record);
}

// QList<Order> DatabaseHelper::getOrderLikeList(const QString& mess)
// {
//     OrderList.clear();
//     struct soap select_soap;
//     soap_init(&select_soap);
//     soap_set_mode(&select_soap,SOAP_C_UTFSTRING);
//     lkf2__getOrderLikeList res;
//     lkf2__getOrderLikeListResponse rep;
//     res.arg0 = (StringTrans::QString2string(mess));
//     int result = soap_call___lkf1__getOrderLikeList(&select_soap,NULL,NULL,&res,rep);
//     if(!result)
//     {
//         //        qDebug()<<"查询成功";
//         std::vector<lkf2__order*> orderList = rep.return_;
//         for(int i=0 ;i < (int)orderList.size();i++ )
//         {
//             Order* order = transObjects::transOrder(orderList[i]);
//             OrderList.append(order);
//             //            qDebug()<<order->getOrderId();
//         }
//     }
//     return OrderList;
// }

// QList<Order> DatabaseHelper::getOrderListHistory()
// {
//     OrderList.clear();
//     struct soap select_soap;
//     soap_init(&select_soap);
//     soap_set_mode(&select_soap,SOAP_C_UTFSTRING);
//     lkf2__getOrderList res;
//     lkf2__getOrderListResponse rep;
//     int result = soap_call___lkf1__getOrderList(&select_soap,NULL,NULL,&res,rep);
//     //    qDebug()<<result;
//     if(!result)
//     {
//         std::vector<lkf2__order*> orderList = rep.return_;
//         for(int i=0;i<(int)orderList.size();i++)
//         {
//             Order* order = transObjects::transOrder(orderList[i]);
//             OrderList.append(order);
//         }
//         qDebug()<<OrderList.size();
//         for(int i=0;i<OrderList.size();i++)
//         {
//             //            qDebug()<<OrderList[i]->getOrderProductName();
//             //            qDebug()<<OrderList[i]->getOrderCost();
//             //            qDebug()<<OrderList[i]->getOrderTime();
//         }
//     }
//     return OrderList;
// }

// bool DatabaseHelper::updateHistory(const Order& order)
// {
//     struct soap update_soap;
//     soap_init(&update_soap);
//     soap_set_mode(&update_soap,SOAP_C_UTFSTRING);
//     lkf2__updateHistory res;
//     lkf2__updateHistoryResponse rep;
//     lkf2__order * javaorder = transObjects::retransOrder(order);
//     res.arg0 = javaorder;
//     int  result = soap_call___lkf1__updateHistory(&update_soap,NULL,NULL,&res,rep);
//     //    qDebug()<<result;
//     if(!result){
//         qDebug()<<"更新成功";
//     }
// }

// bool DatabaseHelper::deleteShoppingCart(const Order& order)
// {
//     struct soap delete_soap;
//     soap_init(&delete_soap);
//     soap_set_mode(&delete_soap,SOAP_C_UTFSTRING);

//     lkf2__deleteShoppingCart res;
//     lkf2__deleteShoppingCartResponse rep;
//     res.arg0 = transObjects::retransOrder(order);
//     int result = soap_call___lkf1__deleteShoppingCart(&delete_soap,NULL,NULL,&res,rep);
//     if(!result)
//     {
//         //        qDebug()<<"删除成功";
//     }
// }

QList<Product> DatabaseHelper::getProductList(const int& choose)
{
    ProductList.clear();
    getAllRecords("`product`");

    for (const QVariantMap &tmp : allRecords) {
        Product row;
        row.setProductId(tmp.value("product_id").toInt());
        row.setProductName(tmp.value("product_name").toString());
        row.setProductPrice(tmp.value("product_price").toInt());
        row.setProductNum(tmp.value("product_num").toInt());
        row.setProductBuyNum(tmp.value("product_buy_num").toInt());
        row.setProductImage(tmp.value("product_image").toString());
        row.setProductDiscount(tmp.value("product_discount").toFloat());
        ProductList.append(row);
    }

    return ProductList;
}

QList<Product> DatabaseHelper::getProductListByInfo(const Product& product)
{
    ProductList.clear();
    struct soap select_soap;
    soap_init(&select_soap);
    soap_set_mode(&select_soap,SOAP_C_UTFSTRING);
    lkf2__getProductListByInfo res;
    lkf2__getProductListByInfoResponse rep;
    res.arg0  = transObjects::retransProduct(product);
    int result = soap_call___lkf1__getProductListByInfo(&select_soap,NULL,NULL,&res,rep);
    //    qDebug()<<result;
    if(!result)
    {
        std::vector<lkf2__product*> productList = rep.return_;
        //        qDebug()<<productList.size();
        for(int i=0;i<(int)productList.size();i++)
        {
            Product* product = transObjects::transProduct(productList[i]);
            ProductList.append(product);
        }
        qDebug() << ProductList.size();
        for(int i=0;i<ProductList.size();i++)
        {
            //            qDebug() << ProductList[i]->getProductName();
        }
    }
    return ProductList;
}

bool DatabaseHelper::addProduct(const Product& product)
{
    QVariantMap record;
    record["product_id"] = product.getProductId();
    record["product_name"] = product.getProductName();
    record["product_price"] = product.getProductPrice();
    record["product_num"] = product.getProductNum();
    record["product_buy_num"] = product.getProductBuyNum();
    record["product_image"] = product.getProductImage();
    record["product_discount"] = product.getProductDiscount();

    return insertRecord("`product`", record);
}

bool DatabaseHelper::deleteProductByInfo(const Product& product)
{
    struct soap delete_soap;
    soap_init(&delete_soap);
    soap_set_mode(&delete_soap,SOAP_C_UTFSTRING);

    lkf2__deleteProductByInfo res;
    lkf2__deleteProductByInfoResponse rep;
    res.arg0 = transObjects::retransProduct(product);
    int result = soap_call___lkf1__deleteProductByInfo(&delete_soap,NULL,NULL,&res,rep);
    if(!result)
        qDebug()<<"删除成功";
}

bool DatabaseHelper::updateProductByInfo(const Product& product)
{
    struct soap update_soap;
    soap_init(&update_soap);
    soap_set_mode(&update_soap,SOAP_C_UTFSTRING);
    lkf2__updateProductByInfo res;
    lkf2__updateProductByInfoResponse rep;
    lkf2__product * javaproduct = transObjects::retransProduct(product);
    res.arg0 = javaproduct;
    int  result = soap_call___lkf1__updateProductByInfo(&update_soap,NULL,NULL,&res,rep);
    //    qDebug()<<result;
    if(!result)qDebug()<<"更新成功2222";
}

// QList<Product> DatabaseHelper::getProductLikeList(const QString& mess)
// {
//     ProductList.clear();
//     struct soap select_soap;
//     soap_init(&select_soap);
//     soap_set_mode(&select_soap,SOAP_C_UTFSTRING);
//     lkf2__getProductLikeList res;
//     lkf2__getProductLikeListResponse rep;
//     res.arg0 = (StringTrans::QString2string(mess));
//     int result = soap_call___lkf1__getProductLikeList(&select_soap,NULL,NULL,&res,rep);
//     if(!result)
//     {
//         qDebug()<<"查询成功";
//         std::vector<lkf2__product*> productList = rep.return_;
//         for(int i=0 ;i < (int)productList.size();i++ )
//         {
//             Product* Product = transObjects::transProduct(productList[i]);
//             ProductList.append(Product);
//             //            qDebug()<<Product->getProductId();
//         }
//     }
//     return ProductList;
// }

QSqlDatabase& DatabaseHelper::getDatabase()
{
    return db;
}
