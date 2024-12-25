#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QVector>
#include <QString>
#include <QPropertyAnimation>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showNextImage();

private:
    Ui::MainWindow *ui;
    QLabel *imageLabel;
    QTimer *timer;
    QVector<QString> imagePaths; // 存储图片路径的向量
    int currentIndex; // 当前显示的图片索引
};
#endif // MAINWINDOW_H
