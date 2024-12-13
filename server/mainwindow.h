#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tcpserver.h"

#include <QDebug>
#include <QMainWindow>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>

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

private:

    Ui::MainWindow *ui;

    TcpServer *server;
    QList<qintptr>socketDiscriptorList;

};
#endif // MAINWINDOW_H
