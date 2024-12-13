#ifndef DATABASEHELPER_H
#define DATABASEHELPER_H

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

    bool isValidTableName(const QString &tableName);

    QSqlDatabase& getDatabase();

private:
    static QString databaseDriver;
    QSqlDatabase db;
    QSqlQuery result;
};

#endif // DATABASEHELPER_H
