#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include "databasehelper.h"
#include "order.h"
#include "product.h"
#include "tcpserver.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMainWindow>
#include <QMutex>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>

// 定义双方接收消息的信号
#define initAll 1000001
#define updateOrder 1000002
#define deleteShopping 1000003
#define searchProduct 1000004
#define addOrderList 1000005
#define chatRoom 1000006
#define searchProductType 1000007
#define clientUpdate 1000008
#define login 1000009
#define signin 1000010
#define updateProfile 1000011

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setSlots();

public slots:

    void dealMessage(QByteArray message,qintptr socketDiscriptor);

private:

    Ui::MainWindow *ui;

    DatabaseHelper *dbhlp;
    static qintptr AsocketDiscriptor;
    QMutex mutex;
    QList<qintptr>socketDiscriptorList;
    TcpServer *server;

};
#endif // MAINWINDOW_H
