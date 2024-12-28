#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "client.h"
#include "databasehelper.h"
#include "order.h"
#include "product.h"
#include "tcpserver.h"

#include <algorithm>
#include <QApplication>
#include <QtCharts>
#include <QDebug>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QListView>
#include <QListWidget>
#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QMutex>
#include <QPainter>
#include <qpainter.h>
#include <QPainterPath>
#include <QPoint>
#include <QRegion>
#include <QScrollBar>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
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
#define withDrawProduct 1000014

using namespace std;

const QString defultImage = "C:/Users/Rito-492/Documents/Study/OOP/E-commerce-system/imagebase/addPic.png";

const int HomeTab = 0;
const int ResultTab = 1;
const int ShoppingRecordsTab = 2;
const int SupportTab = 3;
const int CommodityTab = 4;
const int AnalyseTab = 5;

const int ChatTabs = 5;
const int AnalyseDays = 30;

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

protected:

    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

public slots:

    void dealMessage(QByteArray message,qintptr socketDiscriptor);

private slots:
    void on_minimizeButton_clicked();

    void on_closeButton_clicked();

    void on_searchButton_clicked();

    void on_tabWidget_tabBarClicked(int index);

    void on_returnButton_clicked();

    void on_refreshButton_clicked();

    void on_addPicButton_clicked();

    void edit_commodity();

    void on_saveProductButton_clicked();

    void on_addProductButton_clicked();

    void on_shoppingRecordButton_clicked();

    void on_supportButton_clicked();

    void on_homeButton_clicked();

    void on_iconButton_clicked();

    void on_sendButton_clicked();

    void on_removeProductButton_clicked();

    void on_themeButton_clicked();

    void on_sortButton1_clicked();

    void on_sortButton2_clicked();

    void on_sortButton11_clicked();

    void on_sortButton22_clicked();

    void on_analyseButton_clicked();

private:

    // 初始化一切
    void init();

    // 建立连接
    void setConnections();

    Ui::MainWindow *ui;

    // 实现窗口拖拽
    QPoint m_dragPosition;

    Product curProduct;

    QList<Product> allProducts;

    QList<Product> resultProducts;

    QChartView *view;

    QString curPic;

    QStandardItemModel *model;

    DatabaseHelper *dbhlp;

    static qintptr AsocketDiscriptor;

    QList<qintptr> socketDiscriptorList;

    QMap<qintptr, QListWidget *> chatListWidgets;

    QMap<int, qintptr> chatTab;

    QMap<int, qintptr> sockets;

    bool isUsedTab[ChatTabs];

    int chats;

    int theme;

    int sortBtn1Clicks;

    int sortBtn2Clicks;

    int sortBtn11Clicks;

    int sortBtn22Clicks;

    TcpServer *server;

    QMutex mutex;

    stack<int> lastTab;
};
#endif // MAINWINDOW_H
