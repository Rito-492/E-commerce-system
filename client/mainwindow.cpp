#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpsocket.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isLogging(false)
    , isLogged(false)
    , loginWindow(nullptr)
    , theme(0)
{
    // 设置固定的窗口大小
    this->setFixedSize(800,600);

    // 设置窗口的圆角半径
    const int cornerRadius = 15;

    // 创建一个圆角矩形的路径
    QPainterPath path;
    path.addRoundedRect(rect(), cornerRadius, cornerRadius);

    // 使用路径创建一个区域，并设置为窗口的遮罩
    setMask(QRegion(path.toFillPolygon().toPolygon()));

    // 设置 Ui
    ui->setupUi(this);

    // 初始化一切
    init();

    // 设置连接
    setConnections();

    // refreshTabWidget();
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(rect(), 15, 15);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete socket;
    delete loginWindow;
}

void MainWindow::init() {

    on_themeButton_clicked();

    // 设置 Socket 与服务器连接
    socket = new TcpSocket;
    socket->connectToHost("127.0.0.1", 8284);

    if (!socket->waitForConnected(3000)) {
        QMessageBox::information(nullptr, "Prompt", "Failed to connect to the server.");
    }

    // 设置登录窗口
    loginWindow = new Login;

    ui->tabWidget->setStyleSheet("QTabBar::tab { height: 0px; max-height: 0px; min-height: 0px; }"
                                 "QTabWidget::pane { border: none; }"
                                 );

    // 初始化客服界面指针
    supportTab = ui->tabWidget->widget(SupportTab);

    supportListWidget = supportTab->findChild<QListWidget *>("supportListWidget");

    supportListWidget->setIconSize(QSize(20, 20));

    // 初始化购买记录界面指针
    shoppingRecordsTab = ui->tabWidget->widget(ShoppingRecordsTab);

    shoppingRecordsListWidget = shoppingRecordsTab->findChild<QListWidget *>("shoppingRecordsListWidget");

    shoppingRecordsListWidget->setIconSize(QSize(20, 20));

    // 初始化搜索结果界面指针
    resultTab = ui->tabWidget->widget(ResultTab);

    resultListWidget = resultTab->findChild<QListWidget *>("resultListWidget");

    resultListWidget->setIconSize(QSize(150, 150));

    // 初始化商品界面指针
    commodityTab = ui->tabWidget->widget(CommodityTab);

    // 初始化搜索结果界面指针
    purchaseTab = ui->tabWidget->widget(PurchaseTab);

    purchaseListWidget = purchaseTab->findChild<QListWidget *>("purchaseListWidget");

    purchaseListWidget->setIconSize(QSize(50, 50));

    on_tabWidget_tabBarClicked(HomeTab);
}

void MainWindow::setConnections() {

    // 处理信号消息
    connect(socket, &TcpSocket::sendAByteArray, this, &MainWindow::dealMessage);

    connect(loginWindow, &Login::loginNow, this, &MainWindow::loginNow);
    connect(this, &MainWindow::loginSuccessfully, loginWindow, &Login::loginSuccessfully);

    connect(loginWindow, &Login::signinNow, this, &MainWindow::signinNow);
    connect(this, &MainWindow::signinSuccessfully, loginWindow, &Login::signinSuccessfully);

    connect(ui->titleBar->findChild<QLineEdit *>("searchLineEdit"), &QLineEdit::returnPressed, this, &MainWindow::on_searchButton_clicked);

    connect(resultListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
                QString tmp(item->text());
                QString id;
                for (int i = 6; i < tmp.length() && tmp[i] != '\n'; i++)
                    id += tmp[i];
                curProduct.setProductId(id.toInt());
                show_commodity();
                // for (const Product& product : resultProducts) {
                //     if (product.getProductId() == id.toInt()) {
                //         curProduct = product;
                //         show_commodity();
                //         break;
                //     }
                // }
            });
}

void MainWindow::dealMessage(const QByteArray message) {

    // 解析JSON消息（这里使用QJsonObject进行简单解析）
    QJsonObject jsonObj = QJsonDocument::fromJson(message).object();
    QString signal = jsonObj["signal"].toString();

    if (signal.isEmpty()) {

        qDebug() << "Parsing failed";
        // 发射通知或执行其他错误处理
        return;

    } else if (signal == QString::number(login)) {

        curClient = Client::fromJsonObject(jsonObj);
        if (curClient.getClientId() == 0) {
            QMessageBox::information(nullptr, "登陆失败", "用户不存在或者密码错误！");
        } else {
            isLogged = true;
            isLogging = false;
            QMessageBox::information(nullptr, "登陆成功", "登陆成功！");
            ui->loginButton->setIcon(QIcon(curClient.getClientImage()));
            emit loginSuccessfully();
        }
        on_homeButton_clicked();
        return ;

    } else if (signal == QString::number(signin)) {

        if (!jsonObj["isValid"].toBool()) {
            QMessageBox::information(nullptr, "注册失败", "用户已存在！");
        } else {
            curClient = Client::fromJsonObject(jsonObj);
            isLogged = true;
            isLogging = false;
            QMessageBox::information(nullptr, "注册成功", "已登陆！");
            ui->loginButton->setIcon(QIcon(curClient.getClientImage()));
            emit signinSuccessfully();
        }
        on_homeButton_clicked();
        return ;

    } else if (signal == QString::number(updateProfile)) {

        if (jsonObj["isValid"].toBool()) {
            curClient = Client::fromJsonObject(jsonObj);
            QMessageBox::information(nullptr, "Prompt", "保存成功！");
        } else {
            QMessageBox::information(nullptr, "Prompt", "保存失败！");
        }
        on_profileButton_clicked();
        return ;

    } else if (signal == QString::number(chatRoom)) {

        QListWidgetItem *itemSupport = new QListWidgetItem(QIcon("C:/Users/Rito-492/Documents/Study/OOP/E-commerce-system/client/icons/support.png"), "客服");
        supportListWidget->addItem(itemSupport);

        QString toSendText = jsonObj["message"].toString();
        supportListWidget->addItem(toSendText);

        supportListWidget->addItem("");
        supportTab->findChild<QTextEdit *>("toSendTextEdit")->setText("");

        // 使listWidget滚动到底部
        supportListWidget->scrollToBottom();
        return ;

    } else if (signal == QString::number(getOrders)) {

        if (jsonObj["order_client_id"].toInt() == curClient.getClientId()) {
            QList<Order> orders(Order::fromJsonObjectArray(jsonObj));
            for (const Order& tmp : orders) {
                QString demo = QString("订单编号: %1\n商品编号: %2\n商品名称: %3\n用户名: %4\n购买时间: %5\n金额: ￥%6\n").arg(QString::number(tmp.getOrderId()), QString::number(tmp.getOrderProductId()), tmp.getOrderProductName(), tmp.getOrderClient(), tmp.getOrderTime().toString("yyyy-MM-dd"), tmp.getOrderCost());
                QListWidgetItem *item = new QListWidgetItem(demo);
                shoppingRecordsListWidget->addItem(item);
                shoppingRecordsListWidget->addItem("");
            }

            // 使listWidget滚动到底部
            shoppingRecordsListWidget->scrollToBottom();
        }
        return ;

    } else if (signal == QString::number(searchProduct)) {
        resultProducts = Product::fromJsonObjectArray(jsonObj);
        for (const Product& tmp : resultProducts) {
            QString demo = QString("商品编号: %1\n商品名称: %2\n商品价格: ￥%3\n折后价: ￥%4\n商品剩余量: %5\n").arg(QString::number(tmp.getProductId()), tmp.getProductName(), QString::number(tmp.getProductPrice()), QString::number((int)(tmp.getProductPrice() * tmp.getProductDiscount())), QString::number(tmp.getProductNum()));
            QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(tmp.getProductImage()), demo);
            resultListWidget->addItem(itemProduct);
            resultListWidget->addItem("");
        }
        return ;

    } else if (signal == QString::number(refreshProduct)) {

        curProduct = Product::fromJsonObject(jsonObj);

        ui->spinBox->setValue(1);

        QPixmap pix(curProduct.getProductImage());
        QLabel *imageLabel = commodityTab->findChild<QLabel *>("commodityPicLabel");
        imageLabel->setPixmap(pix.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        commodityTab->findChild<QLabel *>("commodityNameShowLabel")->setText(curProduct.getProductName());
        commodityTab->findChild<QLabel *>("commodityIdShowLabel")->setText(QString::number(curProduct.getProductId()));
        int price = curProduct.getProductPrice() * curProduct.getProductDiscount();
        commodityTab->findChild<QLabel *>("commodityPriceShowLabel")->setText(QString::number(curProduct.getProductPrice()));
        commodityTab->findChild<QLabel *>("commodityDiscountShowLabel")->setText(QString::number(price));
        commodityTab->findChild<QLabel *>("commodityRemainShowLabel")->setText(QString::number(curProduct.getProductNum()));
        commodityTab->findChild<QLabel *>("subtotalShowLabel")->setText(QString("￥%1").arg(QString::number(price)));

        return ;

    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        m_dragPosition = event->globalPosition().toPoint() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::on_minimizeButton_clicked()
{
    this->showMinimized();
}


void MainWindow::on_closeButton_clicked()
{
    this->close();
}


void MainWindow::on_loginButton_clicked()
{
    // if (loginWindow)
    //     return ;
    // else if (!isLogged)
    //     isLogging = true;

    if (isLogged) {
        // 跳转个人中心
        on_profileButton_clicked();
    } else {
        // 准备登录
        loginWindow->show();
    }
}

void MainWindow::loginNow(const QString userName, const QString userPwd) {

    QJsonObject jsonObj;
    jsonObj["signal"] = QString::number(login);
    jsonObj["client_name"] = userName;
    jsonObj["client_pwd"] = userPwd;

    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

}

void MainWindow::signinNow(const QString userName, const QString userPwd) {

    QJsonObject jsonObj;
    jsonObj["signal"] = QString::number(signin);
    jsonObj["client_name"] = userName;
    jsonObj["client_pwd"] = userPwd;

    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

}

void MainWindow::on_profileButton_clicked()
{
    ui->loginButton->setIcon(QIcon(curClient.getClientImage()));

    // 刷新个人中心
    QWidget *profileTab = ui->tabWidget->widget(ProfileTab);
    profileTab->findChild<QPushButton *>("avatarButton")->setIcon(QIcon(curClient.getClientImage()));
    profileTab->findChild<QPushButton *>("avatarButton")->setIconSize(QSize(100, 100));
    profileTab->findChild<QLabel *>("userIdShowLabel")->setText(QString::number(curClient.getClientId()));
    profileTab->findChild<QLineEdit *>("userNameLineEdit")->setText(curClient.getClientName());
    profileTab->findChild<QLineEdit *>("phoneLineEdit")->setText(curClient.getClientPhone());
    profileTab->findChild<QLabel *>("boughtShowLabel")->setText(QString::number(curClient.getClientBought()));
    profileTab->findChild<QLabel *>("signTimeShowLabel")->setText(curClient.getClientSignTime().toString());
    profileTab->findChild<QLineEdit *>("avatarLineEdit")->setText(curClient.getClientImage());
    profileTab->findChild<QLineEdit *>("passWordLineEdit")->setText(curClient.getClientPwd());

    on_tabWidget_tabBarClicked(ProfileTab);
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    lastTab.push(index);
    ui->tabWidget->setCurrentIndex(index);
    qDebug() << index;
}


void MainWindow::on_saveButton_clicked()
{
    if (!isLogged) {
        QMessageBox::information(nullptr, "Prompt", "请先登录！");
        return ;
    }

    Client tmp(curClient);
    QWidget *profileTab = ui->tabWidget->widget(ProfileTab);
    tmp.setClientName(profileTab->findChild<QLineEdit *>("userNameLineEdit")->text());
    tmp.setClientPhone(profileTab->findChild<QLineEdit *>("phoneLineEdit")->text());
    tmp.setClientImage(profileTab->findChild<QLineEdit *>("avatarLineEdit")->text());
    tmp.setClientPwd(profileTab->findChild<QLineEdit *>("passWordLineEdit")->text());

    QJsonObject jsonObj(Client::toJsonObject(tmp));
    jsonObj["signal"] = QString::number(updateProfile);
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

}

void MainWindow::on_sendButton_clicked()
{

    QListWidgetItem *itemClient = new QListWidgetItem(QIcon(curClient.getClientImage()), curClient.getClientName());
    //itemClient->setTextAlignment(Qt::AlignRight);
    supportListWidget->addItem(itemClient);

    QString toSendText = supportTab->findChild<QTextEdit *>("toSendTextEdit")->toPlainText();
    QListWidgetItem *itemText = new QListWidgetItem(toSendText);
    itemText->setTextAlignment(Qt::AlignRight);
    supportListWidget->addItem(toSendText);

    supportListWidget->addItem("");

    supportTab->findChild<QTextEdit *>("toSendTextEdit")->setText("");

    // 使listWidget滚动到底部
    supportListWidget->scrollToBottom();

    QJsonObject jsonObj;
    jsonObj["signal"] = QString::number(chatRoom);
    jsonObj["message"] = toSendText;
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

}


void MainWindow::on_themeButton_clicked()
{
    switch (theme) {
    case 0:
        this->setStyleSheet(
            "alternate-background-color: rgb(255, 219, 254);"
            "font: 14pt \"华文中宋\";"
        );
        break;
    case 1:
        this->setStyleSheet(
            "alternate-background-color: rgb(254, 255, 187);"
            "font: 14pt \"华文中宋\";"
        );
        break;
    case 2:
        this->setStyleSheet(
            "background-color: rgb(243, 198, 255);"
            "font: 14pt \"华文中宋\";"
            "QPushButoon {"
            "border:1px solid rgb(253,253,253); "
            "background-color: rgb(252, 175, 73); "
            "color: rgb(255, 255, 255);"
            "border-radius:15px;"
           " font-family: \"微软雅黑\";"
            "border-color: rgb(255, 255, 255);"

            "}"
            "QPushButton::hover#sendButton {"
            "   background-color: lightblue; /* 鼠标悬停时背景变为浅蓝色 */"
            "   color: darkblue;            /* 鼠标悬停时文本变为深蓝色 */"
            "   border: 2px solid darkblue; /* 鼠标悬停时边框变为2像素宽的深蓝色实线 */"
            "}"
        );
        break;
    }

    //theme++;
    if (theme == 3) theme = 0;

    //QMessageBox::information(nullptr, "", QString::number(theme));
}


void MainWindow::on_shoppingRecordButton_clicked()
{
    shoppingRecordsListWidget->clear();

    QJsonObject jsonObj(Client::toJsonObject(curClient));
    jsonObj["signal"] = QString::number(getOrders);
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

    on_tabWidget_tabBarClicked(ShoppingRecordsTab);
}

void MainWindow::on_refreshButton_clicked()
{

    // const int HomeTab = 0;
    // const int ResultTab = 1;
    // const int ShoppingCartTab = 2;
    // const int ShoppingRecordsTab = 3;
    // const int SupportTab = 4;
    // const int ProfileTab = 5;
    // const int CommodityTab = 6;
    // const int PurchaseTab = 7;

    int index = ui->tabWidget->currentIndex();
    switch (index) {
        case HomeTab:
            on_homeButton_clicked();
            break;
        case ResultTab:
            on_searchButton_clicked();
            break;
        case ShoppingCartTab:
            break;
        case ShoppingRecordsTab:
            on_shoppingRecordButton_clicked();
            break;
        case SupportTab:
            on_supportButton_clicked();
            break;
        case ProfileTab:
            on_profileButton_clicked();
            break;
        case CommodityTab:
            show_commodity();
    }

}


void MainWindow::on_searchButton_clicked()
{
    resultListWidget->clear();

    QString toSearchName = ui->searchLineEdit->text();
    QJsonObject jsonObj;
    jsonObj["signal"] = QString::number(searchProduct);
    jsonObj["toSearchProductName"] = toSearchName;
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

    on_tabWidget_tabBarClicked(ResultTab);
}


void MainWindow::on_supportButton_clicked()
{
    on_tabWidget_tabBarClicked(SupportTab);
}

void MainWindow::show_commodity() {

    QJsonObject jsonObj;
    jsonObj["signal"] = QString::number(refreshProduct);
    jsonObj["product_id"] = curProduct.getProductId();
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

    on_tabWidget_tabBarClicked(CommodityTab);
}

void MainWindow::on_spinBox_valueChanged(int arg1)
{
    int price = commodityTab->findChild<QLabel *>("commodityDiscountShowLabel")->text().toInt();
    commodityTab->findChild<QLabel *>("subtotalShowLabel")->setText(QString("￥%1").arg(QString::number(arg1 * price)));
}


void MainWindow::on_returnButton_clicked()
{
    if (lastTab.size() > 1) {
        lastTab.pop();
        ui->tabWidget->setCurrentIndex(lastTab.top());
    }
}


void MainWindow::on_iconButton_clicked()
{
    while (!lastTab.empty())    lastTab.pop();
    on_tabWidget_tabBarClicked(HomeTab);
}


void MainWindow::on_homeButton_clicked()
{
    while (!lastTab.empty())    lastTab.pop();
    on_tabWidget_tabBarClicked(HomeTab);
}

void MainWindow::on_purchaseButton_clicked()
{


    purchaseTab->findChild<QLineEdit *>("nameLineEdit")->setText(curClient.getClientName());
    purchaseTab->findChild<QLineEdit *>("phoneNumLineEdit")->setText(curClient.getClientPhone());

    on_tabWidget_tabBarClicked(PurchaseTab);
}

