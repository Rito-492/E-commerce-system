#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpsocket.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isLogging(false)
    , isLogged(false)
    , loginWindow(nullptr)
    , carousel(nullptr)
    , theme(0)
    , echoMode(false)
{
    // 设置固定的窗口大小
    this->setFixedSize(1000,750);

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
    // 创建一个QJsonObject
    QJsonObject jsonObj;

    QJsonArray jsonArray1;
    for (const Order& order : productsInCart) {
        jsonArray1.append(Order::toJsonObject(order));
    }
    jsonObj["orders"] = jsonArray1;

    QJsonArray jsonArray2;
    for (const Product& product : pickedProducts) {
        jsonArray2.append(Product::toJsonObject(product));
    }
    jsonObj["products"] = jsonArray2;

    jsonObj["address"] = curClient.getClientAddress();

    // 将QJsonObject封装到QJsonDocument中
    QJsonDocument jsonDoc(jsonObj);

    // 将QJsonDocument转换为字符串并写入文件
    QFile file(QString("%1/data/config-%2.json").arg(PATH, QString::number(curClient.getClientId())));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file for writing:" << file.errorString();
    }

    QTextStream out(&file);
    out << jsonDoc.toJson(QJsonDocument::Indented);
    file.close();

    delete ui;
    delete socket;
    delete loginWindow;
    delete model;
    delete carousel;
}

void MainWindow::init() {

    // 设置 Socket 与服务器连接
    socket = new TcpSocket;
    socket->connectToHost("127.0.0.1", 8284);

    if (!socket->waitForConnected(3000)) {
        QMessageBox::information(nullptr, "Prompt", "Failed to connect to the server.");
    }

    // 设置登录窗口
    loginWindow = new Login();

    model = new QStandardItemModel;

    ui->tabWidget->setStyleSheet("QTabBar::tab { height: 0px; max-height: 0px; min-height: 0px; }"
                                 "QTabWidget::pane { border: none; }"
                                 );

    ui->shoppingRecordsTableView->setStyleSheet(
        "font: 11pt \"华文中宋\";"
        );

    ui->supportListWidget->setIconSize(QSize(20, 20));

    ui->resultListWidget->setIconSize(QSize(150, 150));

    ui->purchaseListWidget->setIconSize(QSize(100, 100));

    ui->demoListWidget->setIconSize(QSize(100, 100));

    ui->historyListWidget->setIconSize(QSize(100, 100));

    ui->cartListView->setIconSize(QSize(100, 100));

    on_themeButton_clicked();

    on_homeButton_clicked();

}

void MainWindow::setConnections() {

    // 处理信号消息
    connect(socket, &TcpSocket::sendAByteArray, this, &MainWindow::dealMessage);

    connect(loginWindow, &Login::loginNow, this, &MainWindow::loginNow);
    connect(this, &MainWindow::loginSuccessfully, loginWindow, &Login::loginSuccessfully);

    connect(loginWindow, &Login::signinNow, this, &MainWindow::signinNow);
    connect(this, &MainWindow::signinSuccessfully, loginWindow, &Login::signinSuccessfully);

    connect(ui->titleBar->findChild<QLineEdit *>("searchLineEdit"), &QLineEdit::returnPressed, this, &MainWindow::on_searchButton_clicked);

    connect(ui->titleBar->findChild<QLineEdit *>("searchLineEdit"), &QLineEdit::textChanged, this, &MainWindow::on_searchButton_clicked);

    connect(ui->resultListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        QString tmp(item->text());
        QString id;
        for (int i = 10; i < tmp.length() && tmp[i] != '\n'; i++)
            id += tmp[i];
        curProduct.setProductId(id.toInt());
        show_commodity();
    });

    connect(ui->demoListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        QString tmp(item->text());
        QString id;
        for (int i = 10; i < tmp.length() && tmp[i] != '\n'; i++)
            id += tmp[i];
        curProduct.setProductId(id.toInt());
        show_commodity();
    });

    connect(ui->historyListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        QString tmp(item->text());
        QString id;
        for (int i = 10; i < tmp.length() && tmp[i] != '\n'; i++)
            id += tmp[i];
        curProduct.setProductId(id.toInt());
        show_commodity();
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
            initCartAndHistory();
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
            curClient.setClientAddress(ui->clientAddressLineEdit->text());
            QMessageBox::information(nullptr, "Prompt", "保存成功！");
        } else {
            QMessageBox::information(nullptr, "Prompt", "保存失败！");
        }
        on_profileButton_clicked();
        return ;

    } else if (signal == QString::number(chatRoom)) {

        QDateTime time = QDateTime::fromString(jsonObj["time"].toString());
        if (QDateTime::currentDateTime().toSecsSinceEpoch() - time.toSecsSinceEpoch() > 120) {
            QListWidgetItem *itemTime = new QListWidgetItem(jsonObj["time"].toString());
            itemTime->setTextAlignment(Qt::AlignCenter);
            ui->supportListWidget->addItem(itemTime);
        }

        QListWidgetItem *itemSupport = new QListWidgetItem(QIcon("C:/Users/Rito-492/Documents/Study/OOP/E-commerce-system/client/icons/support.png"), "客服");
        ui->supportListWidget->addItem(itemSupport);

        QString toSendText = jsonObj["message"].toString();
        ui->supportListWidget->addItem(toSendText);

        ui->supportListWidget->addItem("");
        ui->toSendTextEdit->setText("");

        // 使listWidget滚动到底部
        ui->supportListWidget->scrollToBottom();
        return ;

    } else if (signal == QString::number(getOrders)) {

        if (jsonObj["order_client_id"].toInt() == curClient.getClientId()) {

            QList<Order> orders(Order::fromJsonObjectArray(jsonObj));
            model->clear();
            model->setColumnCount(8);
            model->setHorizontalHeaderLabels({"订单编号", "商品编号", "商品名称", "用户编号", "用户名称", "交易数量", "实付款", "支付时间", "售后"});

            // 添加带有复选框的项到模型
            for (int row = 0; row < orders.size(); row++) {
                const Order& order = orders[row];
                QStandardItem *item = new QStandardItem(QString::number(order.getOrderId()));
                item->setCheckable(true);
                item->setCheckState(Qt::Unchecked); // 初始状态为未选中
                model->setItem(row, 0, item);
                model->setItem(row, 1, new QStandardItem(QString::number(order.getOrderProductId())));
                model->setItem(row, 2, new QStandardItem(order.getOrderProductName()));
                model->setItem(row, 3, new QStandardItem(QString::number(order.getOrderClientId())));
                model->setItem(row, 4, new QStandardItem(order.getOrderClient()));
                model->setItem(row, 5, new QStandardItem(QString::number(order.getOrderProductNum())));
                model->setItem(row, 6, new QStandardItem(order.getOrderCost()));
                model->setItem(row, 7, new QStandardItem(order.getOrderTime().toString("yyyy-MM-dd")));
                model->setItem(row, 8, new QStandardItem(order.getOrderHide() ? "已退货" : "无"));
            }

            // 将模型设置到 QListView
            ui->shoppingRecordsTableView->setModel(model);

            ui->shoppingRecordsTableView->horizontalHeader()->setSortIndicatorShown(true);
            ui->shoppingRecordsTableView->setSortingEnabled(true);

        }
        return ;

    } else if (signal == QString::number(searchProduct)) {
        resultProducts = Product::fromJsonObjectArray(jsonObj);
        // sort(resultProducts.begin(), resultProducts.end(), Product::cmpBySales);
        for (const Product& tmp : resultProducts) {
            QString demo = QString("    商品编号: %1\n    商品名称: %2\n    商品价格: ￥%3\n    折后价: ￥%4\n    商品剩余量: %5\n").arg(QString::number(tmp.getProductId()), tmp.getProductName(), QString::number(tmp.getProductPrice()), QString::number((int)(tmp.getProductPrice() * tmp.getProductDiscount())), QString::number(tmp.getProductNum()));
            QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(tmp.getProductImage()), demo);
            ui->resultListWidget->addItem(itemProduct);
            ui->resultListWidget->addItem("");
        }
        return ;

    } else if (signal == QString::number(refreshProduct)) {

        curProduct = Product::fromJsonObject(jsonObj);
        curProduct.setPickedTime(QDateTime::currentDateTime());

        ui->spinBox->setValue(1);

        QPixmap pix(curProduct.getProductImage());
        ui->commodityPicLabel->setPixmap(pix.scaled(ui->commodityPicLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->commodityNameShowLabel->setText(curProduct.getProductName());
        ui->commodityIdShowLabel->setText(QString::number(curProduct.getProductId()));
        int price = curProduct.getProductPrice() * curProduct.getProductDiscount();
        ui->commodityPriceShowLabel->setText(QString::number(curProduct.getProductPrice()));
        ui->commodityDiscountShowLabel->setText(QString::number(price));
        ui->commodityRemainShowLabel->setText(QString::number(curProduct.getProductNum()));
        ui->subtotalShowLabel->setText(QString::number(price));

        pickedProducts.append(curProduct);
        return ;

    } else if (signal == QString::number(addOrderList)) {
        if (jsonObj["isValid"].toBool()) {
            QMessageBox::information(nullptr, "prompt", "购买成功!");
            for (const Order& toDel : toPayOrders) {
                for (int i = 0; i < productsInCart.size(); i++) {
                    if (productsInCart[i].getOrderProductId() == toDel.getOrderProductId()) {
                        productsInCart.removeAt(i);
                        break;
                    }
                }
            }
            curProduct.setProductId(0);
            toPayOrders.clear();
            on_homeButton_clicked();
        }
    } else if (signal == QString::number(getAllProducts)) {
        ui->demoListWidget->clear();

        allProducts = Product::fromJsonObjectArray(jsonObj);
        sort(allProducts.begin(), allProducts.end(), Product::cmpBySales);

        QVector<QString> imagesPath;
        QList<Product>::iterator it = allProducts.begin();
        for (int i = 1; i <= Carousel::maxImages && it != allProducts.end(); i++, it++) {
            imagesPath.push_back(it->getProductImage());
        }

        if (carousel)
            delete carousel;
        carousel = new Carousel(imagesPath, this);
        connect(carousel, &Carousel::imageClicked, this, [this](const int index) {
            curProduct.setProductId(allProducts[index].getProductId());
            show_commodity();
        });

        ui->homeVBoxLayout->insertWidget(0, carousel);

        for (; it != allProducts.end(); it++) {
            QString demo = QString("    商品编号: %1\n    商品名称: %2\n    商品价格: ￥%3\n    折后价: ￥%4\n    商品剩余量: %5\n").arg(QString::number(it->getProductId()), it->getProductName(), QString::number(it->getProductPrice()), QString::number((int)(it->getProductPrice() * it->getProductDiscount())), QString::number(it->getProductNum()));
            QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(it->getProductImage()), demo);
            ui->demoListWidget->addItem(itemProduct);
            ui->demoListWidget->addItem("");
        }

        return ;
    } else if (signal == QString::number(withDrawProduct)) {
        if (jsonObj["isValid"].toBool()) {
            QMessageBox::information(nullptr, "prompt", "退货成功!");
        } else {
            QMessageBox::information(nullptr, "prompt", "退货失败!");
        }
        on_refreshButton_clicked();
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

    curAvatar = curClient.getClientImage();
    ui->loginButton->setIcon(QIcon(curAvatar));

    QPixmap pixmap(curAvatar);
    pixmap = pixmap.scaled(ui->avatarButton->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->avatarButton->setIcon(QIcon(pixmap));

    ui->userIdShowLabel->setText(QString::number(curClient.getClientId()));
    ui->userNameLineEdit->setText(curClient.getClientName());
    ui->phoneLineEdit->setText(curClient.getClientPhone());
    ui->clientAddressLineEdit->setText(curClient.getClientAddress());
    ui->signTimeShowLabel->setText(curClient.getClientSignTime().toString("yyyy-MM-dd"));
    ui->passWordLineEdit->setText(curClient.getClientPwd());
    ui->passWordLineEdit->setEchoMode(QLineEdit::Password);

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
    tmp.setClientName(ui->userNameLineEdit->text());
    tmp.setClientPhone(ui->phoneLineEdit->text());
    tmp.setClientImage(curAvatar);
    tmp.setClientPwd(ui->passWordLineEdit->text());

    QJsonObject jsonObj(Client::toJsonObject(tmp));
    jsonObj["signal"] = QString::number(updateProfile);
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

}

void MainWindow::on_sendButton_clicked()
{
    if (curClient.getClientId() == 0) {
        QMessageBox::warning(nullptr, "Prompt", "请先登录!");
        return ;
    }

    QListWidgetItem *itemTime = new QListWidgetItem(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    itemTime->setTextAlignment(Qt::AlignCenter);
    ui->supportListWidget->addItem(itemTime);

    QListWidgetItem *itemClient = new QListWidgetItem(QIcon(curClient.getClientImage()), curClient.getClientName());
    //itemClient->setTextAlignment(Qt::AlignRight);
    ui->supportListWidget->addItem(itemClient);

    QString toSendText = ui->toSendTextEdit->toPlainText();
    QListWidgetItem *itemText = new QListWidgetItem(toSendText);
    itemText->setTextAlignment(Qt::AlignRight);
    ui->supportListWidget->addItem(toSendText);

    ui->supportListWidget->addItem("");

    ui->toSendTextEdit->setText("");

    // 使listWidget滚动到底部
    ui->supportListWidget->scrollToBottom();

    QJsonObject jsonObj(Client::toJsonObject(curClient));
    jsonObj["signal"] = QString::number(chatRoom);
    jsonObj["message"] = toSendText;
    jsonObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

}


void MainWindow::on_themeButton_clicked()
{
    switch (theme) {
    case 0:
        this->setStyleSheet(
            "QWidget#centralwidget {"
            "background-color: rgb(255, 255, 255);"
            "font: 14pt \"华文中宋\";"
            "color: rgb(255, 255, 255);"
            "}"
            //247, 249, 252
            "QPushButton {"
            "font: 14pt \"华文中宋\";"
            "background-color: rgb(240, 240, 240);"
            "border-radius:5px;"
            "}"

            "QLabel {"
            "font: 14pt \"华文中宋\";"
            "}"

            "QLineEdit {"
            "font: 14pt \"华文中宋\";"
            "border-radius:4px;"
            "background-color: rgb(240, 240, 240);"
            "}"

            "QTextEdit {"
            "font: 14pt \"华文中宋\";"
            "border-radius:10px;"
            "background-color: rgb(240, 240, 240);"
            "}"

            "QListWidget {"
            "font: 14pt \"华文中宋\";"
            "border-radius:10px;"
            "background-color: rgb(240, 240, 240);"
            "}"

            "QTableView {"
            "font: 14pt \"华文中宋\";"
            "border-radius:10px;"
            "background-color: rgb(240, 240, 240);"
            "}"

            "QListView {"
            "font: 14pt \"华文中宋\";"
            "border-radius:10px;"
            "background-color: rgb(240, 240, 240);"
            "}"
        );
        break;
    case 2:
        this->setStyleSheet(
            "QWidget#centralwidget {"
            "background-color: rgb(254, 255, 187);"
            "font: 14pt \"华文隶书\";"
            "color: rgb(255, 255, 255);"
            "}"

            "QPushButton {"
            "font: 14pt \"华文隶书\";"
            "background-color: rgb(252, 175, 73);"
            "border-radius:5px;"
            "}"

            "QLabel {"
            "font: 14pt \"华文隶书\";"
            "}"

            "QLineEdit {"
            "font: 14pt \"华文隶书\";"
            "border-radius:4px;"
            "background-color: rgb(252, 175, 73);"
            "}"

            "QTextEdit {"
            "font: 14pt \"华文隶书\";"
            "border-radius:10px;"
            "background-color: rgb(252, 175, 73);"
            "}"

            "QListWidget {"
            "font: 14pt \"华文隶书\";"
            "border-radius:10px;"
            "background-color: rgb(252, 175, 73);"
            "}"

            "QTableView {"
            "font: 14pt \"华文隶书\";"
            "border-radius:10px;"
            "background-color: rgb(252, 175, 73);"
            "}"

            "QListView {"
            "font: 14pt \"华文隶书\";"
            "border-radius:10px;"
            "background-color: rgb(252, 175, 73);"
            "}"
        );
        break;
    case 3:
        this->setStyleSheet(
            "QWidget#centralwidget {"
            "background-color: rgb(249, 169, 255);"
            "font: 14pt \"华文新魏\";"
            "color: rgb(255, 255, 255);"
            "}"

            "QPushButton {"
            "font: 14pt \"华文新魏\";"
            "background-color: rgb(255, 205, 253);"
            "border-radius:5px;"
            "}"

            "QLabel {"
            "font: 14pt \"华文新魏\";"
            "}"

            "QLineEdit {"
            "font: 14pt \"华文新魏\";"
            "border-radius:4px;"
            "background-color: rgb(255, 205, 253);"
            "}"

            "QTextEdit {"
            "font: 14pt \"华文新魏\";"
            "border-radius:10px;"
            "background-color: rgb(255, 205, 253);"
            "}"

            "QListWidget {"
            "font: 14pt \"华文新魏\";"
            "border-radius:10px;"
            "background-color: rgb(255, 205, 253);"
            "}"

            "QTableView {"
            "font: 14pt \"华文新魏\";"
            "border-radius:10px;"
            "background-color: rgb(255, 205, 253);"
            "}"

            "QListView {"
            "font: 14pt \"华文新魏\";"
            "border-radius:10px;"
            "background-color: rgb(255, 205, 253);"
            "}"

        );
        break;
    case 1:
        this->setStyleSheet(
            "QWidget#centralwidget {"
            "background-color: rgb(19, 19, 26);"
            "font: 14pt \"华文中宋\";"
            "color: rgb(255, 255, 255);"
            "}"
            //247, 249, 252
            "QPushButton {"
            "font: 14pt \"华文中宋\";"
            "background-color: rgb(54, 54, 62);"
            "color: rgb(255, 255, 255);"
            "border-radius:5px;"
            "}"

            "QLabel {"
            "font: 14pt \"华文中宋\";"
            "color: rgb(255, 255, 255);"
            "}"

            "QLineEdit {"
            "font: 14pt \"华文中宋\";"
            "border-radius:4px;"
            "background-color: rgb(54, 54, 62);"
            "color: rgb(255, 255, 255);"
            "}"

            "QTextEdit {"
            "font: 14pt \"华文中宋\";"
            "border-radius:10px;"
            "background-color: rgb(54, 54, 62);"
            "color: rgb(255, 255, 255);"
            "}"

            "QListWidget {"
            "font: 14pt \"华文中宋\";"
            "border-radius:10px;"
            "background-color: rgb(54, 54, 62);"
            "color: rgb(255, 255, 255);"
            "}"

            "QTableView {"
            "font: 14pt \"华文中宋\";"
            "border-radius:10px;"
            "background-color: rgb(54, 54, 62);"
            "color: rgb(255, 255, 255);"
            "}"

            "QListView {"
            "font: 14pt \"华文中宋\";"
            "border-radius:10px;"
            "background-color: rgb(54, 54, 62);"
            "color: rgb(255, 255, 255);"
            "}"
            );
        break;
    }
    qDebug() << "theme: " << theme << Qt::endl;

    theme++;
    if (theme == 4) theme = 0;

    //QMessageBox::information(nullptr, "", QString::number(theme));
}


void MainWindow::on_shoppingRecordButton_clicked()
{

    QJsonObject jsonObj(Client::toJsonObject(curClient));
    jsonObj["signal"] = QString::number(getOrders);
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

    on_tabWidget_tabBarClicked(ShoppingRecordsTab);
}

void MainWindow::on_refreshButton_clicked()
{

    int index = ui->tabWidget->currentIndex();
    switch (index) {
        case HomeTab:
            on_homeButton_clicked();
            break;
        case ResultTab:
            on_searchButton_clicked();
            break;
        case ShoppingCartTab:
            on_supportButton_clicked();
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
    sortBtn1Clicks = sortBtn2Clicks = 0;

    ui->resultListWidget->clear();

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
    ui->supportListWidget->scrollToBottom();
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
    int price = ui->commodityDiscountShowLabel->text().toInt();
    ui->subtotalShowLabel->setText(QString::number(arg1 * price));
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

    QJsonObject jsonObj;
    jsonObj["signal"] = QString::number(getAllProducts);
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);
}

void MainWindow::on_purchaseButton_clicked()
{
    ui->purchaseListWidget->clear();

    if (curClient.getClientId() == 0) {
        QMessageBox::warning(nullptr, "Prompt", "请先登录!");
        return ;
    }

    if (curProduct.getProductId()) {
        Order order;
        order.setOrderClient(curClient.getClientName());
        order.setOrderClientId(curClient.getClientId());
        order.setOrderProductName(curProduct.getProductName());
        order.setOrderProductId(curProduct.getProductId());
        order.setOrderProductImage(curProduct.getProductImage());
        if (ui->spinBox->value() <= curProduct.getProductNum())
        order.setOrderProductNum(ui->spinBox->value());
        order.setOrderCost(ui->subtotalShowLabel->text());
        toPayOrders.clear();
        toPayOrders.append(order);
    }

    int total = 0;
    for (const Order& order : toPayOrders) {
        QString demo = QString("商品编号: %1\n商品名称: %2\n购买数量: %3\n小计: ￥%4\n").arg(QString::number(order.getOrderProductId()), order.getOrderProductName(), QString::number(order.getOrderProductNum()), order.getOrderCost());
        QListWidgetItem *itemOrder = new QListWidgetItem(QIcon(order.getOrderProductImage()), demo);
        ui->purchaseListWidget->addItem(itemOrder);
        ui->purchaseListWidget->addItem("");
        total += order.getOrderCost().toInt();
    }

    ui->nameLineEdit->setText(curClient.getClientName());
    ui->phoneNumLineEdit->setText(curClient.getClientPhone());
    ui->addressLineEdit->setText(curClient.getClientAddress());
    ui->totalLabel->setText(QString::number(total));

    on_tabWidget_tabBarClicked(PurchaseTab);
}


void MainWindow::on_payButton_clicked()
{

    QJsonObject jsonObj(Order::toJsonObjectArray(toPayOrders));
    jsonObj["signal"] = QString::number(addOrderList);
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);
}


void MainWindow::on_shoppingCartButton_clicked()
{

    model->clear();

    // 添加带有复选框的项到模型
    for (const Order &tmp : productsInCart) {
        QString demo = QString("商品编号: %1\n商品名称: %2\n购买数量: %3\n小计: ￥%4\n").arg(QString::number(tmp.getOrderProductId()), tmp.getOrderProductName(), QString::number(tmp.getOrderProductNum()), tmp.getOrderCost());
        QStandardItem *item = new QStandardItem(QIcon(tmp.getOrderProductImage()), demo);
        item->setCheckable(true); // 设置项为可检查
        item->setCheckState(Qt::Unchecked); // 初始状态为未选中
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled); // 设置项标志
        model->appendRow(item); // 将项添加到模型
    }

    // 将模型设置到 QListView
    ui->cartListView->setModel(model);

    on_tabWidget_tabBarClicked(ShoppingCartTab);
}


void MainWindow::on_addToCartButton_clicked()
{
    if (curClient.getClientId() == 0) {
        QMessageBox::warning(nullptr, "Prompt", "请先登录!");
        return ;
    }

    Order order;
    order.setOrderClient(curClient.getClientName());
    order.setOrderClientId(curClient.getClientId());
    order.setOrderProductName(curProduct.getProductName());
    order.setOrderProductId(curProduct.getProductId());
    order.setOrderProductImage(curProduct.getProductImage());
    if (ui->spinBox->value() <= curProduct.getProductNum())
        order.setOrderProductNum(ui->spinBox->value());
    order.setOrderCost(ui->subtotalShowLabel->text());
    productsInCart.append(order);
    QMessageBox::information(nullptr, "prompt", "添加成功!即将跳转至购物车!");
    on_shoppingCartButton_clicked();
}

void MainWindow::initCartAndHistory() {
    // 初始化购物车和浏览记录
    QFile file(QString("%1/data/config-%2.json").arg(PATH, QString::number(curClient.getClientId())));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        QString jsonString = in.readAll();
        file.close();

        // 解析JSON字符串
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8());
        QJsonObject jsonObj = jsonDoc.object();

        curClient.setClientAddress(jsonObj["address"].toString());

        QList<Order> orders(Order::fromJsonObjectArray(jsonObj));
        for (const Order& order : orders) {
            productsInCart.append(order);
        }

        QList<Product> products(Product::fromJsonObjectArray(jsonObj));
        for (const Product& product : products) {
            pickedProducts.append(product);
        }
    } else {
        qDebug() << "Failed to open file for reading.";
    }
}

void MainWindow::on_purchaseButton2_clicked()
{
    toPayOrders.clear();
    curProduct.setProductId(0);

    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem *item = model->item(row);
        if (item && item->isCheckable() && item->checkState() == Qt::Checked) {
            QString tmp(item->text());
            QString id;
            for (int i = 6; i < tmp.length() && tmp[i] != '\n'; i++)
                id += tmp[i];
            for (const Order& tmp : productsInCart) {
                if (tmp.getOrderProductId() == id.toInt()) {
                    toPayOrders.append(tmp);
                    break;
                }
            }
        }
    }

    on_purchaseButton_clicked();
}


void MainWindow::on_clearButton_clicked()
{
    productsInCart.clear();
    on_shoppingCartButton_clicked();
}

void MainWindow::on_avatarButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择头像文件", "", "Images (*.png *.jpg *.bmp)");
    if (fileName.isEmpty()) {
        // 用户取消了选择
        return;
    }

    // 加载图片
    QPixmap pixmap(fileName);
    if (pixmap.isNull()) {
        // 图片加载失败
        QMessageBox::warning(this, "错误", "无法加载图片文件！");
        return;
    }

    // 调整图片大小以适应标签（可选）
    pixmap = pixmap.scaled(ui->avatarButton->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->avatarButton->setIcon(QIcon(pixmap));

    curAvatar = fileName;
}

void MainWindow::on_withDrawButton_clicked()
{
    QList<Order> toWithDrawOrders;
    for (int i = 0; i < model->rowCount(); i++) {
        QModelIndex index = model->index(i, 0);
        QVariant checkState = model->data(index, Qt::CheckStateRole);
        if (checkState.canConvert<Qt::CheckState>() && (checkState.value<Qt::CheckState>() == Qt::Checked)) {
            Order order;
            order.setOrderId(model->data(index).toInt());
            toWithDrawOrders.append(order);
        }
    }

    QJsonObject jsonObj(Order::toJsonObjectArray(toWithDrawOrders));
    jsonObj["signal"] = QString::number(withDrawProduct);
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

}


void MainWindow::on_sortButton1_clicked()
{
    ui->resultListWidget->clear();

    sort(resultProducts.begin(), resultProducts.end(), Product::cmpBySales);

    if (sortBtn1Clicks & 1)
        reverse(resultProducts.begin(), resultProducts.end());

    for (const Product& tmp : resultProducts) {
        QString demo = QString("    商品编号: %1\n    商品名称: %2\n    商品价格: ￥%3\n    折后价: ￥%4\n    商品剩余量: %5\n").arg(QString::number(tmp.getProductId()), tmp.getProductName(), QString::number(tmp.getProductPrice()), QString::number((int)(tmp.getProductPrice() * tmp.getProductDiscount())), QString::number(tmp.getProductNum()));
        QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(tmp.getProductImage()), demo);
        ui->resultListWidget->addItem(itemProduct);
        ui->resultListWidget->addItem("");
    }

    sortBtn1Clicks++;
}

void MainWindow::on_sortButton2_clicked()
{
    ui->resultListWidget->clear();

    sort(resultProducts.begin(), resultProducts.end(), Product::cmpByPrice);

    if (sortBtn2Clicks & 1)
        reverse(resultProducts.begin(), resultProducts.end());

    for (const Product& tmp : resultProducts) {
        QString demo = QString("    商品编号: %1\n    商品名称: %2\n    商品价格: ￥%3\n    折后价: ￥%4\n    商品剩余量: %5\n").arg(QString::number(tmp.getProductId()), tmp.getProductName(), QString::number(tmp.getProductPrice()), QString::number((int)(tmp.getProductPrice() * tmp.getProductDiscount())), QString::number(tmp.getProductNum()));
        QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(tmp.getProductImage()), demo);
        ui->resultListWidget->addItem(itemProduct);
        ui->resultListWidget->addItem("");
    }

    sortBtn2Clicks++;
}


void MainWindow::on_historyButton_clicked()
{
    ui->historyListWidget->clear();

    for (const Product& tmp : pickedProducts) {
        ui->historyListWidget->addItem(tmp.getPickedTime().toString("yyyy-MM-dd hh:mm:ss"));
        QString demo = QString("    商品编号: %1\n    商品名称: %2\n    商品价格: ￥%3\n    折后价: ￥%4\n    商品剩余量: %5\n").arg(QString::number(tmp.getProductId()), tmp.getProductName(), QString::number(tmp.getProductPrice()), QString::number((int)(tmp.getProductPrice() * tmp.getProductDiscount())), QString::number(tmp.getProductNum()));
        QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(tmp.getProductImage()), demo);
        ui->historyListWidget->addItem(itemProduct);
        ui->historyListWidget->addItem("");
    }

    // 使listWidget滚动到底部
    ui->historyListWidget->scrollToBottom();

    on_tabWidget_tabBarClicked(HistoryTab);
}


void MainWindow::on_echoModePushButton_clicked()
{
    if (echoMode) {
        ui->echoModePushButton->setIcon(QIcon(":/icons/NoPassword.png"));
        ui->passWordLineEdit->setEchoMode(QLineEdit::Password);
    } else {
        ui->echoModePushButton->setIcon(QIcon(":/icons/Password.png"));
        ui->passWordLineEdit->setEchoMode(QLineEdit::Normal);
    }
    echoMode = !echoMode;
}
