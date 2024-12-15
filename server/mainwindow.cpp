#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "databasehelper.h"
#include "tcpserver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dbhlp.connectToDatabase("localhost", 3306, "ecommerce", "root", "8284RT<>");

    // 创建一个 QVariantMap 来存储 WHERE 子句的条件
    QVariantMap whereFields;
    whereFields["client_id"] = 1;

    // 调用 selectRecords 函数并获取结果
    QList<QVariantMap> users = dbhlp.selectRecords("client", whereFields);

    // 遍历结果并打印出来
    for (const QVariantMap &user : users) {
        qDebug() << "ID:" << user.value("client_id").toInt();
        qDebug() << "Name:" << user.value("client_name").toString();
        qDebug() << "Phone:" << user.value("client_phone").toString();
    }

    server = new TcpServer(this, 8284);

}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
}
