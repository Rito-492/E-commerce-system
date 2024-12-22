#include "login.h"
#include "ui_login.h"

bool Login::isLogging = false;

Login::Login(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);

    connect(ui->usePwdLineEdit, &QLineEdit::returnPressed, this, &Login::on_loginButton_clicked);
}

Login::~Login()
{
    delete ui;
}

void Login::on_loginButton_clicked()
{

    QString userName = ui->userNameLineEdit->text();
    QString userPwd = ui->usePwdLineEdit->text();

    if (userName.isEmpty() || userPwd.isEmpty()) {
        QMessageBox::information(nullptr, "Prompt", "用户名和密码不能为空！");
    } else {
        emit loginNow(userName, userPwd);
    }

}

void Login::loginSuccessfully()
{
    this->close();
}

void Login::on_signinButton_clicked()
{
    QString userName = ui->userNameLineEdit->text();
    QString userPwd = ui->usePwdLineEdit->text();

    if (userName.isEmpty() || userPwd.isEmpty()) {
        QMessageBox::information(nullptr, "Prompt", "用户名和密码不能为空！");
    } else {
        emit signinNow(userName, userPwd);
    }
}

void Login::signinSuccessfully()
{
    this->close();
}
