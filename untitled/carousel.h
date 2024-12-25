#ifndef CAROUSEL_H
#define CAROUSEL_H

#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QVector>
#include <QWidget>

class Carousel : public QWidget {
    Q_OBJECT

public:
    explicit Carousel(const QVector<QString> &imagePaths, QWidget *parent = nullptr);

private slots:
    void nextImage();
    void prevImage();
    void jumpToImage(int index);
    void updateDots();

private:
    QVector<QPixmap> images; // 存储图片
    QLabel *imageLabel; // 显示当前图片的标签
    QPushButton *nextButton; // 下一个图片按钮
    QPushButton *prevButton; // 上一个图片按钮
    QVector<QPushButton *> dots; // 底部点
    QTimer *timer; // 定时器
    int currentImageIndex; // 当前显示的图片索引
};

#endif // CAROUSEL_H
