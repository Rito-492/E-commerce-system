#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentIndex(0)
{
    ui->setupUi(this);

    // 获取界面中的QLabel控件
    imageLabel = ui->label; // 假设在设计器中QLabel的名字是label

    // 初始化图片路径
    imagePaths = {
        ":/images/image1.jpg", // 使用资源文件路径或绝对路径
        ":/images/image2.jpg",
        ":/images/image3.jpg"
    };

    // 设置初始图片
    imageLabel->setPixmap(QPixmap(imagePaths[currentIndex]));

    // 创建并启动定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::showNextImage);
    timer->start(3000); // 每3秒切换一次图片
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showNextImage()
{
    currentIndex = (currentIndex + 1) % imagePaths.size(); // 循环显示图片
    imageLabel->setPixmap(QPixmap(imagePaths[currentIndex]));
}
