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

//INSERT INTO 表名([字段1,字段2..])VALUES('值1','值2'..),[('值1','值2'..)..];
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

//UPDATE 表名 SET 字段1=值1,[字段2=值2...] WHERE 条件[];
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

//DELETE FROM 表名 [WHERE 条件]
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

//SELECT 查询列表 FROM 表名;
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

QSqlDatabase& DatabaseHelper::getDatabase()
{
    return db;
}
