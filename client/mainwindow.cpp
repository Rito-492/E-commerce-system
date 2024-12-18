#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpsocket.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isLogging(false)
    , isLogged(false)
    , loginWindow(nullptr)
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


    // 设置 Socket 与服务器连接
    socket = new TcpSocket;
    socket->connectToHost("127.0.0.1", 8284);

    if (!socket->waitForConnected(3000)) {
        QMessageBox::information(nullptr, "Prompt", "Failed to connect to the server.");
    }


    // 设置登录窗口
    loginWindow = new Login;

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
    delete ui;
    delete socket;
    delete loginWindow;
}

void MainWindow::setConnections() {

    connect(loginWindow, &Login::loginNow, this, &MainWindow::loginNow);
    connect(this, &MainWindow::loginSuccessfully, loginWindow, &Login::loginSuccessfully);
    connect(socket, &TcpSocket::sendAByteArray, this, &MainWindow::loginResult);
    connect(loginWindow, &Login::signinNow, this, &MainWindow::signinNow);
    connect(this, &MainWindow::signinSuccessfully, loginWindow, &Login::signinSuccessfully);
    connect(socket, &TcpSocket::sendAByteArray, this, &MainWindow::signinResult);
    connect(socket, &TcpSocket::sendAByteArray, this, &MainWindow::toUpdateProfile);

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

void MainWindow::loginResult(const QByteArray datagram) {

    QJsonObject jsonObj = QJsonDocument::fromJson(datagram).object();
    QString signal = jsonObj["signal"].toString();
    if (signal == QString::number(login)) {
        curClient = Client::fromJsonObject(jsonObj);
        if (curClient.getClientId() == 0) {
            QMessageBox::information(nullptr, "登陆失败", "用户不存在或者密码错误！");
        } else {
            isLogged = true;
            isLogging = false;
            refreshProfileWidget();
            QMessageBox::information(nullptr, "登陆成功", "登陆成功！");
            emit loginSuccessfully();
        }
    }

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

void MainWindow::signinResult(const QByteArray datagram) {

    QJsonObject jsonObj = QJsonDocument::fromJson(datagram).object();
    QString signal = jsonObj["signal"].toString();
    if (signal == QString::number(signin)) {
        if (!jsonObj["isValid"].toBool()) {
            QMessageBox::information(nullptr, "注册失败", "用户已存在！");
        } else {
            curClient = Client::fromJsonObject(jsonObj);
            isLogged = true;
            isLogging = false;
            refreshProfileWidget();
            QMessageBox::information(nullptr, "注册成功", "已登陆！");
            emit signinSuccessfully();
        }
    }

}

void MainWindow::refreshProfileWidget() {

    ui->loginButton->setIcon(QIcon(curClient.getClientImage()));

    QWidget *profileWidget = ui->tabWidget->widget(ProfileWidget);
    profileWidget->findChild<QPushButton *>("avatarButton")->setIcon(QIcon(curClient.getClientImage()));
    profileWidget->findChild<QPushButton *>("avatarButton")->setIconSize(QSize(100, 100));
    profileWidget->findChild<QPushButton *>("avatarButton")->setIconSize(QSize(100, 100));
    profileWidget->findChild<QLabel *>("userIdShowLabel")->setText(QString::number(curClient.getClientId()));
    profileWidget->findChild<QLineEdit *>("userNameLineEdit")->setText(curClient.getClientName());
    profileWidget->findChild<QLineEdit *>("phoneLineEdit")->setText(curClient.getClientPhone());
    profileWidget->findChild<QLabel *>("boughtShowLabel")->setText(QString::number(curClient.getClientBought()));
    profileWidget->findChild<QLabel *>("signTimeShowLabel")->setText(curClient.getClientSignTime().toString("yy-mm-dd"));
    profileWidget->findChild<QLineEdit *>("avatarLineEdit")->setText(curClient.getClientImage());
    profileWidget->findChild<QLineEdit *>("passWordLineEdit")->setText(curClient.getClientPwd());
}

void MainWindow::on_profileButton_clicked()
{
    refreshProfileWidget();
    on_tabWidget_tabBarClicked(ProfileWidget);
}


void MainWindow::on_tabWidget_tabBarClicked(int index)
{

    ui->tabWidget->setCurrentIndex(index);

}


void MainWindow::on_saveButton_clicked()
{
    if (!isLogged) {
        QMessageBox::information(nullptr, "Prompt", "请先登录！");
        return ;
    }

    Client tmp(curClient);
    QWidget *profileWidget = ui->tabWidget->widget(ProfileWidget);
    tmp.setClientName(profileWidget->findChild<QLineEdit *>("userNameLineEdit")->text());
    tmp.setClientPhone(profileWidget->findChild<QLineEdit *>("phoneLineEdit")->text());
    tmp.setClientImage(profileWidget->findChild<QLineEdit *>("avatarLineEdit")->text());
    tmp.setClientPwd(profileWidget->findChild<QLineEdit *>("passWordLineEdit")->text());

    QJsonObject jsonObj(Client::toJsonObject(tmp));
    jsonObj["signal"] = QString::number(updateProfile);
    QByteArray jsonString = QJsonDocument(jsonObj).toJson();
    // 发送消息到服务器
    socket->sendMessageToServer(jsonString);

}

void MainWindow::toUpdateProfile(const QByteArray datagram) {
    QJsonObject jsonObj = QJsonDocument::fromJson(datagram).object();
    QString signal = jsonObj["signal"].toString();
    if (signal == QString::number(updateProfile)) {
        if (jsonObj["isValid"].toBool()) {
            curClient = Client::fromJsonObject(jsonObj);
            refreshProfileWidget();
            QMessageBox::information(nullptr, "Prompt", "保存成功！");
        } else {
            refreshProfileWidget();
            QMessageBox::information(nullptr, "Prompt", "保存失败！");
        }
    }
}

