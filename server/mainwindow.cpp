#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "tcpserver.h"
#include "databasehelper.h"

#include <QSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlDriver>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    DatabaseHelper dbhlp;
    dbhlp.connectToDatabase("localhost", 3306, "ecommerce", "root", "8284RT<>");

    QStringList tables = dbhlp.getDatabase().tables();
    foreach(QString table, tables)
        qDebug()<<table;

    TcpServer server;
}

MainWindow::~MainWindow()
{
    delete ui;
}
