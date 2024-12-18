#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "databasehelper.h"
#include "tcpserver.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server = new TcpServer(this, 8284);

    this->setSlots();

    dbhlp = new DatabaseHelper;
    dbhlp->connectToDatabase("localhost", 3306, "ecommerce", "root", "8284RT<>");

}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
    delete dbhlp;
}

void MainWindow::setSlots()
{
    connect(server, &TcpServer::sglReceivedReomClient, this, [this](const QByteArray& message, const qintptr& socketDiscriptor) {
            this->dealMessage(message, socketDiscriptor);
            qDebug() <<"我的信号为："<< QThread::currentThread() << Qt::endl << Qt::endl;
            });

    // connect(this,&MainWindow::sglSendByteArray,[=](QByteArray json,qintptr socketDiscriptor){
    //     server->dataSendFromServer(json,socketDiscriptor);
    // });


    // connect(server,&TcpServer::sendDisconnectedSignal,[=](qintptr socketDiscriptor){
    //     setNotification(2,QString::number(socketDiscriptor)+"断开连接");
    // });
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

    } else if (signal == QString::number(searchProduct)) {
        /*
            规定：由产品名称查询
        */
        QString productName = jsonObj["product_name"].toString();
        Product tmp(dbhlp->getProductByName(productName));
        QJsonObject responseObj = Product::toJsonObject(tmp);
        responseObj["signal"] = signal;
        QByteArray responseMessage = QJsonDocument(responseObj).toJson();
        emit server->dataSendFromServer(responseMessage, socketDescriptor);
        return;

    } else if (signal == QString::number(login)) {

        QString clientName = jsonObj["client_name"].toString();
        QString clientPwd = jsonObj["client_pwd"].toString();
        Client tmp(dbhlp->getClientByNameAndPwd(clientName, clientPwd));
        QJsonObject responseObj = Client::toJsonObject(tmp);
        responseObj["signal"] = signal;
        if (tmp.getClientName() == clientName && tmp.getClientPwd() == clientPwd)
            responseObj["isValid"] = 1;
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

        QByteArray responseMessage = QJsonDocument(responseObj).toJson();
        emit server->dataSendFromServer(responseMessage, socketDescriptor);
        return ;

    }
}
