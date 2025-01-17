#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "databasehelper.h"
#include "tcpserver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , chats(0)
    , theme(0)
{
    ui->setupUi(this);

    // 设置固定的窗口大小
    this->setFixedSize(1000, 750);

    // 设置窗口的圆角半径
    const int cornerRadius = 15;

    // 创建一个圆角矩形的路径
    QPainterPath path;
    path.addRoundedRect(rect(), cornerRadius, cornerRadius);

    // 使用路径创建一个区域，并设置为窗口的遮罩
    setMask(QRegion(path.toFillPolygon().toPolygon()));

    init();

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
    delete ui;
    delete server;
    delete dbhlp;
    delete model;
}

void MainWindow::init() {

    on_themeButton_clicked();

    server = new TcpServer(this, 8284);

    dbhlp = new DatabaseHelper;

    model = new QStandardItemModel;

    ui->tabWidget->setStyleSheet("QTabBar::tab { height: 0px; max-height: 0px; min-height: 0px; }"
                                  "QTabWidget::pane { border: none; }"
         );

    ui->demoListWidget->setIconSize(QSize(130, 130));

    ui->resultListWidget->setIconSize(QSize(150, 150));

    ui->shoppingRecordsTableView->setStyleSheet(
        "font: 11pt \"华文中宋\";"
        );

    view = new QChartView;

    ui->analyseLayout->addWidget(view);

    on_iconButton_clicked();

}

void MainWindow::setConnections()
{
    connect(server, &TcpServer::sglReceivedReomClient, this, [this](const QByteArray& message, const qintptr& socketDiscriptor) {
            this->dealMessage(message, socketDiscriptor);
            qDebug() <<"我的信号为："<< QThread::currentThread() << Qt::endl << Qt::endl;
            });

    connect(ui->titleBar->findChild<QLineEdit *>("searchLineEdit"), &QLineEdit::returnPressed, this, &MainWindow::on_searchButton_clicked);

    connect(ui->titleBar->findChild<QLineEdit *>("searchLineEdit"), &QLineEdit::textChanged, this, &MainWindow::on_searchButton_clicked);

    connect(ui->demoListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
                QString tmp(item->text());
                QString id;
                for (int i = 10; i < tmp.length() && tmp[i] != '\n'; i++)
                    id += tmp[i];
                curProduct.setProductId(id.toInt());
                edit_commodity();
            });

    connect(ui->resultListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
                QString tmp(item->text());
                QString id;
                for (int i = 10; i < tmp.length() && tmp[i] != '\n'; i++)
                    id += tmp[i];
                curProduct.setProductId(id.toInt());
                edit_commodity();
            });

    connect(ui->usersListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
                QString tmp(item->text());
                QString id;
                for (int i = 0; i < tmp.length() && tmp[i] != ' '; i++)
                        id += tmp[i];
                Client client = dbhlp->getClientById(id.toInt());
                // QMessageBox::information(nullptr, "", client.getClientName());
                for (auto it = chatTab.begin(); it != chatTab.end(); it++) {
                    if (it.value() == sockets[client.getClientId()]) {
                        ui->chatTabWidget->setCurrentIndex(it.key());
                        break;
                    }
                }
    });

    // connect(this,&MainWindow::sglSendByteArray,[=](QByteArray json,qintptr socketDiscriptor){
    //     server->dataSendFromServer(json,socketDiscriptor);
    // });

    connect(server, &TcpServer::sendDisconnectedSignal, [=](qintptr socketDiscriptor) {
        chatListWidgets[socketDiscriptor] = nullptr;
        int client_id = 0;
        for (QMap<int, qintptr>::iterator it = sockets.begin(); it != sockets.end(); it++) {
            if (it.value() == socketDiscriptor) {
                client_id = it.key();
                break;
            }
        }
        for (auto it = chatTab.begin(); it != chatTab.end(); it++) {
            if (it.value() == socketDiscriptor) {
                it.value() = 0;
                isUsedTab[it.key()] = false;
                break;
            }
        }

        for (int i = 0; i < ui->usersListWidget->count(); i++) {
            QListWidgetItem *item = ui->usersListWidget->item(i);
            QString tmp(item->text());
            QString id;
            for (int i = 0; i < tmp.length() && tmp[i] != ' '; i++)
                id += tmp[i];
            if (id.toInt() == client_id) {
                ui->usersListWidget->takeItem(i);
                break;
            }
        }
    });
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

void MainWindow::dealMessage(QByteArray message, qintptr socketDescriptor)
{
    QMutexLocker lock(&mutex);

    // 解析JSON消息（这里使用QJsonObject进行简单解析）
    QJsonObject jsonObj = QJsonDocument::fromJson(message).object();
    QString signal = jsonObj["signal"].toString();

    if (signal.isEmpty()) {

        qDebug() << "Parsing failed";
        // 发射通知或执行其他错误处理
        return;

    } else if (signal == QString::number(login)) {

        QString clientName = jsonObj["client_name"].toString();
        QString clientPwd = jsonObj["client_pwd"].toString();
        Client tmp(dbhlp->getClientByNameAndPwd(clientName, clientPwd));
        QJsonObject responseObj = Client::toJsonObject(tmp);
        responseObj["signal"] = signal;
        if (tmp.getClientName() == clientName && tmp.getClientPwd() == clientPwd) {
            responseObj["isValid"] = 1;
            for (int i = 0; i < ChatTabs; i++) {
                if (!isUsedTab[i]) {
                    for (QObject* child : ui->chatTabWidget->widget(i)->children()) {
                        // 检查子控件是否为QWidget类型（因为QListWidget继承自QWidget）
                        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
                            // 检查QWidget是否为QListWidget类型
                            if (QListWidget* listWidget = qobject_cast<QListWidget*>(widget)) {
                                // 找到QListWidget，返回其指针
                                listWidget->clear();
                                chatListWidgets[socketDescriptor] = listWidget;
                                chatTab[i] = socketDescriptor;
                                break;
                            }
                        }
                    }
                    isUsedTab[i] = true;
                    break;
                }
            }
            sockets[tmp.getClientId()] = socketDescriptor;
            QListWidgetItem *itemClient = new QListWidgetItem(QIcon(tmp.getClientImage()), QString("%1  %2").arg(QString::number(tmp.getClientId()), tmp.getClientName()));
            ui->usersListWidget->addItem(itemClient);
        }
        else
            responseObj["isValid"] = 0;
        QByteArray responseMessage = QJsonDocument(responseObj).toJson();
        emit server->dataSendFromServer(responseMessage, socketDescriptor);
        return;

    } else if (signal == QString::number(signin)) {

        QString clientName = jsonObj["client_name"].toString();
        QString clientPwd = jsonObj["client_pwd"].toString();
        Client tmp(dbhlp->getClientByNameAndPwd(clientName, clientPwd));

        bool flag;
        if (tmp.getClientId() == 0) {
            tmp.setClientName(clientName);
            tmp.setClientPwd(clientPwd);
            flag = dbhlp->insertClient(tmp);
        } else {
            flag = false;
        }

        tmp = dbhlp->getClientByNameAndPwd(clientName, clientPwd);

        if (flag) {
            for (int i = 0; i < ChatTabs; i++) {
                if (!isUsedTab[i]) {
                    for (QObject* child : ui->chatTabWidget->widget(i)->children()) {
                        // 检查子控件是否为QWidget类型（因为QListWidget继承自QWidget）
                        if (QWidget* widget = qobject_cast<QWidget*>(child)) {
                            // 检查QWidget是否为QListWidget类型
                            if (QListWidget* listWidget = qobject_cast<QListWidget*>(widget)) {
                                // 找到QListWidget，返回其指针
                                listWidget->clear();
                                chatListWidgets[socketDescriptor] = listWidget;
                                chatTab[i] = socketDescriptor;
                                break;
                            }
                        }
                    }
                    isUsedTab[i] = true;
                    break;
                }
            }
            QListWidgetItem *itemClient = new QListWidgetItem(QIcon(tmp.getClientImage()), QString("%1  %2").arg(QString::number(tmp.getClientId()), tmp.getClientName()));
            ui->usersListWidget->addItem(itemClient);
        }

        QJsonObject responseObj = Client::toJsonObject(tmp);
        responseObj["signal"] = signal;
        responseObj["isValid"] = flag;
        QByteArray responseMessage = QJsonDocument(responseObj).toJson();
        emit server->dataSendFromServer(responseMessage, socketDescriptor);
        return;

    } else if (signal == QString::number(updateProfile)) {

        Client tmp(Client::fromJsonObject(jsonObj));
        bool flag = dbhlp->updateClient(tmp);
        QJsonObject responseObj = Client::toJsonObject(tmp);
        responseObj["signal"] = signal;
        responseObj["isValid"] = flag;

        if (flag) {
            for (int i = 0; i < ui->usersListWidget->count(); i++) {
                QListWidgetItem *item = ui->usersListWidget->item(i);
                QString str(item->text());
                QString id;
                for (int i = 0; i < str.length() && str[i] != ' '; i++)
                    id += str[i];
                if (id.toInt() == tmp.getClientId()) {
                    item->setIcon(QIcon(tmp.getClientImage()));
                    break;
                }
            }
        }

        QByteArray responseMessage = QJsonDocument(responseObj).toJson();
        emit server->dataSendFromServer(responseMessage, socketDescriptor);
        return ;

    } else if (signal == QString::number(chatRoom)) {
        Client tmp(Client::fromJsonObject(jsonObj));

        QDateTime time = QDateTime::fromString(jsonObj["time"].toString());
        if (QDateTime::currentDateTime().toSecsSinceEpoch() - time.toSecsSinceEpoch() > 120) {
            QListWidgetItem *itemTime = new QListWidgetItem(jsonObj["time"].toString());
            itemTime->setTextAlignment(Qt::AlignCenter);
            chatListWidgets[socketDescriptor]->addItem(itemTime);
        }

        QListWidgetItem *itemClient = new QListWidgetItem(QIcon(tmp.getClientImage()), tmp.getClientName());
        chatListWidgets[socketDescriptor]->addItem(itemClient);

        QString receivedText = jsonObj["message"].toString();
        chatListWidgets[socketDescriptor]->addItem(receivedText);

        chatListWidgets[socketDescriptor]->addItem("");

        // 使listWidget滚动到底部
        chatListWidgets[socketDescriptor]->scrollToBottom();
        return ;

    } else if (signal == QString::number(getOrders)) {

        Client tmp(Client::fromJsonObject(jsonObj));
        QList<Order> orders = dbhlp->getOrderListByInfo(tmp);

        QJsonObject responseObj(Order::toJsonObjectArray(orders));
        responseObj["signal"] = signal;
        responseObj["order_client_id"] = tmp.getClientId();

        QByteArray responseMessage = QJsonDocument(responseObj).toJson();
        emit server->dataSendFromServer(responseMessage, socketDescriptor);
        return ;

    } else if (signal == QString::number(searchProduct)) {

        QString toSearchProductName = jsonObj["toSearchProductName"].toString();
        QList<Product> result = dbhlp->searchProductByName(toSearchProductName);

        QJsonObject responseObj(Product::toJsonObjectArray(result));
        responseObj["signal"] = signal;

        QByteArray responseMessage = QJsonDocument(responseObj).toJson();
        emit server->dataSendFromServer(responseMessage, socketDescriptor);
        return ;

    } else if (signal == QString::number(refreshProduct)) {

        Product product(dbhlp->getProductById(jsonObj["product_id"].toInt()));

        QJsonObject responseObj(Product::toJsonObject(product));
        responseObj["signal"] = signal;

        QByteArray responseMessage = QJsonDocument(responseObj).toJson();
        emit server->dataSendFromServer(responseMessage, socketDescriptor);
        return ;

    } else if (signal == QString::number(addOrderList)) {

        QList<Order> toPayOrders(Order::fromJsonObjectArray(jsonObj));
        bool flag = true;
        for (const Order& tmp : toPayOrders) {
            flag = dbhlp->addOrder(tmp);
        }

        QJsonObject responseObj;
        responseObj["signal"] = signal;
        responseObj["isValid"] = flag;

        QByteArray responseMessage = QJsonDocument(responseObj).toJson();
        emit server->dataSendFromServer(responseMessage, socketDescriptor);

        return ;

    } else if (signal == QString::number(getAllProducts)) {

        QList<Product> result = dbhlp->getProductList();

        QJsonObject responseObj(Product::toJsonObjectArray(result));
        responseObj["signal"] = signal;

        QByteArray responseMessage = QJsonDocument(responseObj).toJson();
        emit server->dataSendFromServer(responseMessage, socketDescriptor);
        return ;

    } else if (signal == QString::number(withDrawProduct)) {
        QList<Order> toWithDrawOrders(Order::fromJsonObjectArray(jsonObj));
        bool flag = true;
        for (Order& tmp : toWithDrawOrders) {
            tmp.setOrderHide(1);
            flag = dbhlp->updateOrderByInfo(tmp);
        }

        QJsonObject responseObj(Order::toJsonObjectArray(toWithDrawOrders));
        responseObj["signal"] = signal;
        responseObj["isValid"] = flag;

        QByteArray responseMessage = QJsonDocument(responseObj).toJson();
        emit server->dataSendFromServer(responseMessage, socketDescriptor);
        return ;
    }
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
            "border-radius:3px;"
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
    case 1:
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
    case 2:
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
    case 3:
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

void MainWindow::on_minimizeButton_clicked()
{
    this->showMinimized();
}


void MainWindow::on_closeButton_clicked()
{
    this->close();
}


void MainWindow::on_searchButton_clicked()
{
    sortBtn11Clicks = sortBtn22Clicks = 0;

    ui->resultListWidget->clear();

    QString toSearchName = ui->searchLineEdit->text();
    resultProducts = dbhlp->searchProductByName(toSearchName);
    for (const Product& tmp : resultProducts) {
        QString demo = QString("    商品编号: %1\n    商品名称: %2\n    商品价格: ￥%3\n    折后价: ￥%4\n    商品剩余量: %5\n").arg(QString::number(tmp.getProductId()), tmp.getProductName(), QString::number(tmp.getProductPrice()), QString::number((int)(tmp.getProductPrice() * tmp.getProductDiscount())), QString::number(tmp.getProductNum()));
        QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(tmp.getProductImage()), demo);
        ui->resultListWidget->addItem(itemProduct);
        ui->resultListWidget->addItem("");
    }

    on_tabWidget_tabBarClicked(ResultTab);
}


void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    lastTab.push(index);
    ui->tabWidget->setCurrentIndex(index);
    qDebug() << index;
}


void MainWindow::on_returnButton_clicked()
{
    if (lastTab.size() > 1) {
        lastTab.pop();
        ui->tabWidget->setCurrentIndex(lastTab.top());
    }
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
    case ShoppingRecordsTab:
        on_shoppingRecordButton_clicked();
        break;
    case SupportTab:
        on_supportButton_clicked();
        break;
    case CommodityTab:
       edit_commodity();
    }

}

void MainWindow::edit_commodity() {


    curProduct = dbhlp->getProductById(curProduct.getProductId());

    if (curProduct.getProductImage().isEmpty()) {
        curPic = defultImage;
    } else {
        curPic = curProduct.getProductImage();
    }

    QPixmap pixmap(curPic);
    pixmap = pixmap.scaled(ui->addPicButton->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->addPicButton->setIcon(QIcon(pixmap));
    ui->productNameLineEdit->setText(curProduct.getProductName());
    ui->commodityIdShowLabel->setText(QString::number(curProduct.getProductId()));
    ui->priceLineEdit->setText(QString::number(curProduct.getProductPrice()));
    ui->discountLineEdit->setText(QString::number(curProduct.getProductDiscount()));
    ui->remainLineEdit->setText(QString::number(curProduct.getProductNum()));
    ui->buyNumLineEdit->setText(QString::number(curProduct.getProductBuyNum()));
    on_tabWidget_tabBarClicked(CommodityTab);
}

void MainWindow::on_addPicButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择图片文件", "", "Images (*.png *.jpg *.bmp *.avif)");
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
    pixmap = pixmap.scaled(ui->addPicButton->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    ui->addPicButton->setIcon(QIcon(pixmap));

    curPic = fileName;
}

void MainWindow::on_saveProductButton_clicked()
{
    Product tmp(curProduct);
    tmp.setProductImage(curPic);
    tmp.setProductName(ui->productNameLineEdit->text());
    tmp.setProductPrice(ui->priceLineEdit->text().toInt());
    tmp.setProductDiscount(ui->discountLineEdit->text().toDouble());
    tmp.setProductNum(ui->remainLineEdit->text().toInt());
    tmp.setProductBuyNum(ui->buyNumLineEdit->text().toInt());

    if (dbhlp->getProductById(tmp.getProductId()).getProductId()) {
        if (dbhlp->updateProductByInfo(tmp)) {
            QMessageBox::information(nullptr, "prompt", "商品修改成功!");
            curProduct = tmp;
        } else {
            QMessageBox::warning(nullptr, "prompt", "商品修改失败!");
        }
    } else {
        int id = dbhlp->addProduct(tmp);
        if (id) {
            QMessageBox::information(nullptr, "prompt", "商品添加成功!");
            curProduct = dbhlp->getProductById(id);
        } else {
            QMessageBox::warning(nullptr, "prompt", "商品添加失败!");
        }
    }

    on_refreshButton_clicked();
}


void MainWindow::on_addProductButton_clicked()
{
    curProduct.setProductId(0);
    edit_commodity();
}


void MainWindow::on_shoppingRecordButton_clicked()
{
    model->clear();
    model->setColumnCount(8);
    model->setHorizontalHeaderLabels({"订单编号", "商品编号", "商品名称", "用户编号", "用户名称", "交易数量", "实付款", "支付时间", "售后"});
    QList<Order> orders(dbhlp->getOrderList());

    // 添加带有复选框的项到模型
    for (int row = 0; row < orders.size(); row++) {
        const Order& order = orders[row];
        model->setItem(row, 0, new QStandardItem(QString::number(order.getOrderId())));
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

    on_tabWidget_tabBarClicked(ShoppingRecordsTab);
}


void MainWindow::on_supportButton_clicked()
{
    on_tabWidget_tabBarClicked(SupportTab);
}


void MainWindow::on_homeButton_clicked()
{
    sortBtn1Clicks = sortBtn2Clicks = 0;

    ui->demoListWidget->clear();
    allProducts = dbhlp->getProductList();
    for (const Product& tmp : allProducts) {
        QString demo = QString("    商品编号: %1\n    商品名称: %2\n    商品价格: ￥%3\n    折后价: ￥%4\n    商品剩余量: %5\n").arg(QString::number(tmp.getProductId()), tmp.getProductName(), QString::number(tmp.getProductPrice()), QString::number((int)(tmp.getProductPrice() * tmp.getProductDiscount())), QString::number(tmp.getProductNum()));
        QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(tmp.getProductImage()), demo);
        ui->demoListWidget->addItem(itemProduct);
        ui->demoListWidget->addItem("");
    }

    on_tabWidget_tabBarClicked(HomeTab);
}

void MainWindow::on_iconButton_clicked()
{
    while (!lastTab.empty())    lastTab.pop();
    on_homeButton_clicked();
}

void MainWindow::on_sendButton_clicked()
{
    qintptr curSocket = chatTab[ui->chatTabWidget->currentIndex()];
    QListWidget *curListWidget = chatListWidgets[curSocket];
    QListWidgetItem *itemClient = new QListWidgetItem(QIcon("C:/Users/Rito-492/Documents/Study/OOP/E-commerce-system/client/icons/support.png"), "客服");
    //itemClient->setTextAlignment(Qt::AlignRight);
    curListWidget->addItem(itemClient);

    QString toSendText = ui->toSendTextEdit->toPlainText();
    QListWidgetItem *itemText = new QListWidgetItem(toSendText);
    itemText->setTextAlignment(Qt::AlignRight);
    curListWidget->addItem(toSendText);

    curListWidget->addItem("");

    ui->toSendTextEdit->setText("");

    // 使listWidget滚动到底部
    curListWidget->scrollToBottom();

    QJsonObject jsonObj;
    jsonObj["signal"] = QString::number(chatRoom);
    jsonObj["message"] = toSendText;
    jsonObj["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    emit server->dataSendFromServer(jsonString, curSocket);
}


void MainWindow::on_removeProductButton_clicked()
{
    if (curProduct.getProductId() == 0) {
        QMessageBox::warning(nullptr, "Warning", "添加商品时无法删除!");
        return ;
    }

    if (QMessageBox::question(nullptr, "Prompt", "确定要删除该商品?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        if (dbhlp->deleteProductByInfo(curProduct)) {
            QMessageBox::information(nullptr, "Prompt", "删除成功!");
        } else {
            QMessageBox::information(nullptr, "Prompt", "删除失败!");
        }
    }
}


void MainWindow::on_sortButton1_clicked()
{
    ui->demoListWidget->clear();

    sort(allProducts.begin(), allProducts.end(), Product::cmpBySales);

    if (sortBtn1Clicks & 1)
        reverse(allProducts.begin(), allProducts.end());

    for (const Product& tmp : allProducts) {
        QString demo = QString("    商品编号: %1\n    商品名称: %2\n    商品价格: ￥%3\n    折后价: ￥%4\n    商品剩余量: %5\n").arg(QString::number(tmp.getProductId()), tmp.getProductName(), QString::number(tmp.getProductPrice()), QString::number((int)(tmp.getProductPrice() * tmp.getProductDiscount())), QString::number(tmp.getProductNum()));
        QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(tmp.getProductImage()), demo);
        ui->demoListWidget->addItem(itemProduct);
        ui->demoListWidget->addItem("");
    }

    sortBtn1Clicks++;
}


void MainWindow::on_sortButton2_clicked()
{
    ui->demoListWidget->clear();

    sort(allProducts.begin(), allProducts.end(), Product::cmpByPrice);

    if (sortBtn2Clicks & 1)
        reverse(allProducts.begin(), allProducts.end());

    for (const Product& tmp : allProducts) {
        QString demo = QString("    商品编号: %1\n    商品名称: %2\n    商品价格: ￥%3\n    折后价: ￥%4\n    商品剩余量: %5\n").arg(QString::number(tmp.getProductId()), tmp.getProductName(), QString::number(tmp.getProductPrice()), QString::number((int)(tmp.getProductPrice() * tmp.getProductDiscount())), QString::number(tmp.getProductNum()));
        QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(tmp.getProductImage()), demo);
        ui->demoListWidget->addItem(itemProduct);
        ui->demoListWidget->addItem("");
    }

    sortBtn2Clicks++;
}


void MainWindow::on_sortButton11_clicked()
{
    ui->resultListWidget->clear();

    sort(resultProducts.begin(), resultProducts.end(), Product::cmpBySales);

    if (sortBtn11Clicks & 1)
        reverse(resultProducts.begin(), resultProducts.end());

    for (const Product& tmp : resultProducts) {
        QString demo = QString("    商品编号: %1\n    商品名称: %2\n    商品价格: ￥%3\n    折后价: ￥%4\n    商品剩余量: %5\n").arg(QString::number(tmp.getProductId()), tmp.getProductName(), QString::number(tmp.getProductPrice()), QString::number((int)(tmp.getProductPrice() * tmp.getProductDiscount())), QString::number(tmp.getProductNum()));
        QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(tmp.getProductImage()), demo);
        ui->resultListWidget->addItem(itemProduct);
        ui->resultListWidget->addItem("");
    }

    sortBtn11Clicks++;
}


void MainWindow::on_sortButton22_clicked()
{
    ui->resultListWidget->clear();

    sort(resultProducts.begin(), resultProducts.end(), Product::cmpByPrice);

    if (sortBtn22Clicks & 1)
        reverse(resultProducts.begin(), resultProducts.end());

    for (const Product& tmp : resultProducts) {
        QString demo = QString("    商品编号: %1\n    商品名称: %2\n    商品价格: ￥%3\n    折后价: ￥%4\n    商品剩余量: %5\n").arg(QString::number(tmp.getProductId()), tmp.getProductName(), QString::number(tmp.getProductPrice()), QString::number((int)(tmp.getProductPrice() * tmp.getProductDiscount())), QString::number(tmp.getProductNum()));
        QListWidgetItem *itemProduct = new QListWidgetItem(QIcon(tmp.getProductImage()), demo);
        ui->resultListWidget->addItem(itemProduct);
        ui->resultListWidget->addItem("");
    }

    sortBtn22Clicks++;
}


void MainWindow::on_analyseButton_clicked()
{
    if (curProduct.getProductId() == 0) {
        QMessageBox::warning(nullptr, "prompt", "添加商品时无法进行分析!");
    }

    QList<Order> orders = dbhlp->getOrderListByProduct(curProduct);

    QMap<QDateTime, int> sales;
    QMap<QDateTime, int> withdraws;

    QDateTime curDate = QDateTime::currentDateTime();

    int i = 0;
    while (i < orders.size()) {
        QDateTime orderDate = orders[i].getOrderTime();
        // 计算日期差（毫秒）
        qint64 msecs = orderDate.toMSecsSinceEpoch() - curDate.toMSecsSinceEpoch();

        // 将毫秒转换为天数
        qint64 days = msecs / (1000 * 60 * 60 * 24);
        if (days <= AnalyseDays) {
            break;
        }
        i++;
    }
    while (i < orders.size()) {
        sales[orders[i].getOrderTime()] += orders[i].getOrderProductNum();
        if (orders[i].getOrderHide() == 1)
            withdraws[orders[i].getOrderTime()] += orders[i].getOrderProductNum();
        else
            withdraws[orders[i].getOrderTime()] += 0;
        i++;
    }

    int maxSales = 0;
    int maxWithdraws = 0;
    for (auto it : sales) {
        maxSales = max(it, maxSales);
    }

    for (auto it : withdraws) {
        maxWithdraws = max(it, maxWithdraws);
    }

    // 创建数据集
    QBarSet *set0 = new QBarSet("销量");
    QBarSet *set1 = new QBarSet("退货");

    for (auto it : sales) {
        set0->append(it);
    }

    for (auto it : withdraws) {
        set1->append(it);
    }

    // 创建柱状图系列
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    series->append(set1);

    // 创建图表并设置系列
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setTitle(QString("%1 近 %2 天销量及售后统计").arg(curProduct.getProductName(), QString::number(AnalyseDays)));

    // 创建分类轴
    QStringList categories;
    for (QMap<QDateTime, int>::iterator it = sales.begin(); it != sales.end(); it++) {
        categories.append(it.key().toString("MM-dd"));
    }

    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, max(maxSales, maxWithdraws) + 3);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 创建图表视图并设置图表
    view->setChart(chart);
    view->setRenderHint(QPainter::Antialiasing);

    on_tabWidget_tabBarClicked(AnalyseTab);
}
