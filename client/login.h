#ifndef LOGIN_H
#define LOGIN_H

#include <QMessageBox>
#include <QMutex>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Login;
}
QT_END_NAMESPACE

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();

signals:

    void loginNow(const QString& userName, const QString& userPwd);

    void signinNow(const QString& userName, const QString& userPwd);

public slots:

    void loginSuccessfully();

    void signinSuccessfully();

private slots:

    void on_loginButton_clicked();

    void on_signinButton_clicked();

private:

    Ui::Login *ui;
    static bool isLogging;
};

#endif // LOGIN_H
