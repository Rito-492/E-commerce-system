#include "databasehelper.h"

QString DatabaseHelper::databaseDriver = "QODBC";

DatabaseHelper::DatabaseHelper(QObject *parent)
    : QObject(parent), db(QSqlDatabase::addDatabase(databaseDriver))
{
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("ecommerce");
    db.setUserName("root");
    db.setPassword("8284RT<>");

    if (!db.open()) {
        // qDebug() << "Error: Unable to establish a database connection.";
        // qDebug() << db.lastError().text();
        // 可以选择在这里显示一个错误消息框给用户
        QMessageBox::critical(nullptr, QObject::tr("Database Connection Error"),
                              db.lastError().text(), QMessageBox::Cancel);
    } else {
        qDebug() << "Database connection established.";
    }

    getOrderList();

}

DatabaseHelper::~DatabaseHelper()
{
    if (db.isOpen())
        db.close();
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

    // // 验证表名不为空
    // if (!isValidTableName(tableName)) {
    //     qWarning() << "Invalid table name format or the tablename is empty!";
    //     return false;
    // }

    // 构建字段和占位符列表
    for (auto it = record.begin(); it != record.end(); ++it) {
        if (!fields.isEmpty()) fields += ", ";
        if (!values.isEmpty()) values += ", ";
        fields += it.key();
        placeholders.append("?");
        values += QString("?");
    }

    // 构建SQL插入语句
    // QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName, fields, values.replace("?", QStringList(placeholders.size(), "?").join(", ")));
    QString sql = QString("INSERT INTO %1 (%2) VALUES (%3)").arg(tableName, fields, values);
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

    // // 验证表名不为空
    // if (!isValidTableName(tableName)) {
    //     qWarning() << "Invalid table name format or the tablename is empty!";
    //     return false;
    // }

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

    // // 验证表名不为空
    // if (!isValidTableName(tableName)) {
    //     qWarning() << "Invalid table name format or the tablename is empty!";
    //     return false;
    // }

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

    // // 验证表名不为空
    // if (!isValidTableName(tableName)) {
    //     qWarning() << "Invalid table name format or the tablename is empty!";
    //     return result;
    // }

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
    // // 验证表名不为空
    // if (tableName.isEmpty()) {
    //     qWarning() << "Table name cannot be empty";
    //     return false;
    // }

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

    // // 验证表名不为空
    // if (!isValidTableName(tableName)) {
    //     qWarning() << "Invalid table name format or the tablename is empty!";
    //     return allRecords;
    // }

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
        for (const QVariantMap &tmp : tmpList) {
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

Client DatabaseHelper::getClientById(const int id)
{
    // 创建一个 QVariantMap 来存储 WHERE 子句的条件
    QVariantMap whereFields;
    whereFields["client_id"] = id;

    // 调用 selectRecords 函数并获取结果
    QList<QVariantMap> tmpList = selectRecords("`client`", whereFields);

    Client row;

    if (tmpList.size() > 1) {
        qDebug() << "There is a wrong !";
    } else {
        for (const QVariantMap &tmp : tmpList) {
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

bool DatabaseHelper::insertClient(const Client client)
{
    QVariantMap record;
    record["client_name"] = client.getClientName();
    record["client_sign_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    record["client_pwd"] = client.getClientPwd();
    record["client_bought"] = 0;
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

bool DatabaseHelper::updateClient(const Client client)
{
    QVariantMap whereFields;
    whereFields["client_id"] = client.getClientId();

    QVariantMap record;
    record["client_name"] = client.getClientName();
    record["client_phone"] = client.getClientPhone();
    record["client_pwd"] = client.getClientPwd();
    record["client_image"] = client.getClientImage();

    return updateRecord("`client`", record, whereFields);
}

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

QList<Order> DatabaseHelper::getOrderListByInfo(const Client client)
{

    // 创建一个 QVariantMap 来存储 WHERE 子句的条件
    QVariantMap whereFields;
    whereFields["order_client_id"] = client.getClientId();

    // 调用 selectRecords 函数并获取结果
    QList<QVariantMap> tmpList = selectRecords("`order`", whereFields);

    QList<Order> orders;
    for (const QVariantMap &tmp : tmpList) {
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
        orders.append(row);
    }

    return orders;
}

QList<Order> DatabaseHelper::getOrderListByProduct(const Product product)
{
    // 创建一个 QVariantMap 来存储 WHERE 子句的条件
    QVariantMap whereFields;
    whereFields["order_product_id"] = product.getProductId();

    // 调用 selectRecords 函数并获取结果
    QList<QVariantMap> tmpList = selectRecords("`order`", whereFields);

    QList<Order> orders;
    for (const QVariantMap &tmp : tmpList) {
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
        orders.append(row);
    }

    return orders;
}

bool DatabaseHelper::addOrder(const Order order)
{
    Client client(getClientById(order.getOrderClientId()));
    int bought = client.getClientBought();
    client.setClientBought(bought + order.getOrderProductNum());
    updateClient(client);

    Product product(getProductById(order.getOrderProductId()));
    int num = product.getProductNum();
    int buy_num = product.getProductBuyNum();
    if (num >= order.getOrderProductNum()) {
        product.setProductNum(num - order.getOrderProductNum());
        product.setProductBuyNum(buy_num + order.getOrderProductNum());
    }
    updateProductByInfo(product);

    QVariantMap record;
    record["order_product_name"] = order.getOrderProductName();
    record["order_product_num"] = order.getOrderProductNum();
    record["order_product_style"] = order.getOrderProductStyle();
    record["order_product_id"] = order.getOrderProductId();
    record["order_cost"] = order.getOrderCost();
    record["order_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    record["order_client"] = order.getOrderClient();·1
    record["order_client_id"] = order.getOrderClientId();
    record["order_check"] = order.getOrderCheck();
    record["order_hide"] = order.getOrderHide();

    return insertRecord("`order`", record);
}

bool DatabaseHelper::updateOrderByInfo(const Order& order)
{
    QVariantMap whereFields;
    whereFields["order_id"] = order.getOrderId();

    QVariantMap record;
    // record["order_product_name"] = order.getOrderProductName();
    // record["order_product_num"] = order.getOrderProductNum();
    // record["order_product_style"] = order.getOrderProductStyle();
    // record["order_product_id"] = order.getOrderProductId();
    // record["order_cost"] = order.getOrderCost();
    // record["order_time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    // record["order_client"] = order.getOrderClient();
    // record["order_client_id"] = order.getOrderClientId();
    // record["order_check"] = order.getOrderCheck();
    record["order_hide"] = order.getOrderHide();

    return updateRecord("`order`", record, whereFields);
}

QList<Product> DatabaseHelper::getProductList()
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

QList<Product> DatabaseHelper::searchProductByName(const QString toSearchProductName) {

    QString sql;
    QSqlQuery sqlQuery(db);
    ProductList.clear();

    // 构建SQL选择语句，使用LIKE进行模糊搜索
    sql = QString("SELECT * FROM product WHERE product_name LIKE ?");
    sqlQuery.prepare(sql);
    sqlQuery.addBindValue(QVariant(QString("%%1%%").arg(toSearchProductName)));

    // 执行选择操作并处理结果
    if (sqlQuery.exec()) {
        while (sqlQuery.next()) {
            QVariantMap row;
            int columnCount = sqlQuery.record().count();
            for (int i = 0; i < columnCount; ++i) {
                row[sqlQuery.record().fieldName(i)] = sqlQuery.value(i);
            }

            Product tmp;
            tmp.setProductId(row["product_id"].toInt());
            tmp.setProductName(row["product_name"].toString());
            tmp.setProductPrice(row["product_price"].toInt());
            tmp.setProductNum(row["product_num"].toInt());
            tmp.setProductBuyNum(row["product_buy_num"].toInt());
            tmp.setProductImage(row["product_image"].toString());
            tmp.setProductDiscount(row["product_discount"].toDouble());

            ProductList.append(tmp);
        }
    } else {
        qWarning() << "Database error:" << sqlQuery.lastError().text()
        << "\nQuery:" << sqlQuery.lastQuery();
    }

    return ProductList;
}

// QList<Product> DatabaseHelper::getProductListByInfo(const Product& product)
// {
//     ProductList.clear();
//     struct soap select_soap;
//     soap_init(&select_soap);
//     soap_set_mode(&select_soap,SOAP_C_UTFSTRING);
//     lkf2__getProductListByInfo res;
//     lkf2__getProductListByInfoResponse rep;
//     res.arg0  = transObjects::retransProduct(product);
//     int result = soap_call___lkf1__getProductListByInfo(&select_soap,NULL,NULL,&res,rep);
//     //    qDebug()<<result;
//     if(!result)
//     {
//         std::vector<lkf2__product*> productList = rep.return_;
//         //        qDebug()<<productList.size();
//         for(int i=0;i<(int)productList.size();i++)
//         {
//             Product* product = transObjects::transProduct(productList[i]);
//             ProductList.append(product);
//         }
//         qDebug() << ProductList.size();
//         for(int i=0;i<ProductList.size();i++)
//         {
//             //            qDebug() << ProductList[i]->getProductName();
//         }
//     }
//     return ProductList;
// }

int DatabaseHelper::addProduct(const Product& product)
{
    QVariantMap record;
    record["product_name"] = product.getProductName();
    record["product_price"] = product.getProductPrice();
    record["product_num"] = product.getProductNum();
    record["product_buy_num"] = product.getProductBuyNum();
    record["product_image"] = product.getProductImage();
    record["product_discount"] = product.getProductDiscount();

    if (insertRecord("`product`", record)) {
        QList<QVariantMap> tmpList = selectRecords("`product`", record);
        if (tmpList.size() != 1) {
            qDebug() << "There is a wrong !";
            return 0;
        } else {
            return tmpList.begin()->value("product_id").toInt();
        }
    } else {
        return 0;
    }
}

Product DatabaseHelper::getProductById(const int& productId)
{

    // 创建一个 QVariantMap 来存储 WHERE 子句的条件
    QVariantMap whereFields;
    whereFields["product_id"] = productId;

    // 调用 selectRecords 函数并获取结果
    QList<QVariantMap> tmpList = selectRecords("product", whereFields);

    Product row;

    if (tmpList.size() > 1) {
        qDebug() << "There is a wrong !";
    } else {
        for (const QVariantMap &tmp : tmpList) {
            row.setProductId(tmp.value("product_id").toInt());
            row.setProductName(tmp.value("product_name").toString());
            row.setProductPrice(tmp.value("product_price").toFloat());
            row.setProductNum(tmp.value("product_num").toInt());
            row.setProductBuyNum(tmp.value("product_buy_num").toInt());
            row.setProductImage(tmp.value("product_image").toString());
            row.setProductDiscount(tmp.value("product_discount").toFloat());
        }
    }

    return row;
}

bool DatabaseHelper::deleteProductByInfo(const Product& product)
{
    // 创建一个 QVariantMap 来存储 WHERE 子句的条件
    QVariantMap whereFields;
    whereFields["product_id"] = product.getProductId();

    return deleteRecord("`product`", whereFields);
}

bool DatabaseHelper::updateProductByInfo(const Product& product)
{
    QVariantMap whereFields;
    whereFields["product_id"] = product.getProductId();

    QVariantMap record;
    record["product_id"] = product.getProductId();
    record["product_name"] = product.getProductName();
    record["product_price"] = product.getProductPrice();
    record["product_num"] = product.getProductNum();
    record["product_buy_num"] = product.getProductBuyNum();
    record["product_image"] = product.getProductImage();
    record["product_discount"] = product.getProductDiscount();

    return updateRecord("`product`", record, whereFields);
}

QSqlDatabase& DatabaseHelper::getDatabase()
{
    return db;
}
