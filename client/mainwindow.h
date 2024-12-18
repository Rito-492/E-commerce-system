#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include "login.h"
#include "tcpsocket.h"

#include <QApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <qpainter.h>
#include <QPainterPath>
#include <QPoint>
#include <QRegion>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

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

const int ProfileWidget = 4;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:

    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:

    void loginSuccessfully();

    void signinSuccessfully();

private slots:

    void on_minimizeButton_clicked();

    void on_closeButton_clicked();

    void on_loginButton_clicked();

    void loginNow(const QString userName, const QString userPwd);

    void loginResult(const QByteArray datagram);

    void signinNow(const QString userName, const QString userPwd);

    void signinResult(const QByteArray datagram);

    void toUpdateProfile(const QByteArray datagram);

    void on_profileButton_clicked();

    void refreshProfileWidget();

    void on_tabWidget_tabBarClicked(int index);

    void on_saveButton_clicked();

private:

    void setConnections();

    Ui::MainWindow *ui;

    // 实现窗口拖拽
    QPoint m_dragPosition;

    // 标记是否正在登录
    bool isLogging;
    // 标记是否已登录
    bool isLogged;
    // 登录窗口指针
    Login *loginWindow;

    // 当前登录的用户信息
    Client curClient;

    TcpSocket *socket;
};

#endif // MAINWINDOW_H
