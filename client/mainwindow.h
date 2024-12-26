#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "carousel.h"
#include "client.h"
#include "login.h"
#include "order.h"
#include "product.h"
#include "tcpsocket.h"
#include "searchline.h"

#include <algorithm>
#include <QApplication>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QList>
#include <QListView>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <qpainter.h>
#include <QPainterPath>
#include <QPoint>
#include <QRegion>
#include <QScrollBar>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QTabWidget>
#include <QTabBar>
#include <QTextEdit>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <stack>

// 定义双方接收消息的信号
#define initAll 1000001
#define updateOrder 1000002
#define deleteShopping 1000003
#define searchProduct 1000004
#define addOrderList 1000005
#define chatRoom 1000006
#define refreshProduct 1000007
#define clientUpdate 1000008
#define login 1000009
#define signin 1000010
#define updateProfile 1000011
#define getOrders 1000012
#define getAllProducts 1000013

using namespace std;

const int HomeTab = 0;
const int ResultTab = 1;
const int ShoppingCartTab = 2;
const int ShoppingRecordsTab = 3;
const int SupportTab = 4;
const int ProfileTab = 5;
const int CommodityTab = 6;
const int PurchaseTab = 7;

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

    void dealMessage(const QByteArray message);

    void on_minimizeButton_clicked();

    void on_closeButton_clicked();

    void on_loginButton_clicked();

    void loginNow(const QString userName, const QString userPwd);

    void signinNow(const QString userName, const QString userPwd);

    void on_profileButton_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_saveButton_clicked();

    void on_sendButton_clicked();

    void on_themeButton_clicked();

    void on_shoppingRecordButton_clicked();

    void on_refreshButton_clicked();

    void on_searchButton_clicked();

    void on_supportButton_clicked();

    void show_commodity();

    void on_spinBox_valueChanged(int arg1);

    void on_returnButton_clicked();

    void on_iconButton_clicked();

    void on_homeButton_clicked();

    void on_purchaseButton_clicked();

    void on_payButton_clicked();

    void on_shoppingCartButton_clicked();

    void on_addToCartButton_clicked();

    void on_purchaseButton2_clicked();

    void on_clearButton_clicked();

    void on_avatarButton_clicked();

private:

    void init();

    void setConnections();

    Ui::MainWindow *ui;

    // 轮播图
    Carousel *carousel;

    // 关键词联想
    SearchLine *searchLine;

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

    // 当前头像
    QString curAvatar;

    // 商品搜索结果列表
    QList<Product> resultProducts;

    // 所有商品
    QList<Product> allProducts;

    // 当前展示的商品
    Product curProduct;

    int theme;

    QList<Order> toPayOrders;

    QList<Order> productsInCart;

    QStandardItemModel *model;

    Order curOrder;

    // 记录上一个界面， 用于返回
    stack<int> lastTab;

    TcpSocket *socket;
};

#endif // MAINWINDOW_H
