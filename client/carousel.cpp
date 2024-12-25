#include "carousel.h"

int Carousel::maxImages = 5;

Carousel::Carousel(const QVector<QString> &imagePaths, QWidget *parent) : QWidget(parent), currentImageIndex(0) {
    // 初始化图片数组
    images.reserve(imagePaths.size());
    for (const QString &path : imagePaths) {
        images.append(QPixmap(path));
    }

    // 创建图片标签
    imageLabel = new ClickableLabel(this);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setFixedSize(width, height); // 固定imageLabel的大小
    imageLabel->setScaledContents(true); // 开启内容缩放
    imageLabel->setPixmap(images.value(0).scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation)); // 设置初始图片
    connect(imageLabel, &ClickableLabel::clicked, this, [this]() {
        emit imageClicked(currentImageIndex);
    });

    // 创建左右箭头按钮
    nextButton = new QPushButton(this);
    prevButton = new QPushButton(this);
    nextButton->setIcon(QIcon(":/icons/right-arrow.png"));
    nextButton->setFixedWidth(30);
    nextButton->setFlat(true);
    nextButton->setCursor(Qt::PointingHandCursor);
    prevButton->setIcon(QIcon(":/icons/left-arrow.png"));
    prevButton->setFixedWidth(30);
    prevButton->setFlat(true);
    prevButton->setCursor(Qt::PointingHandCursor);
    connect(nextButton, &QPushButton::clicked, this, &Carousel::nextImage);
    connect(prevButton, &QPushButton::clicked, this, &Carousel::prevImage);

    // 创建底部点
    for (int i = 0; i < images.size(); ++i) {
        QPushButton *dot = new QPushButton(this);
        dot->setStyleSheet("background-color: gray; border-radius: 4px;");
        dot->setFixedWidth(8);
        dot->setFixedHeight(8);
        dot->setFlat(true); // 去除按钮的边框和背景，使其看起来像点
        dot->setVisible(true);
        dot->setEnabled(true);
        dot->setCursor(Qt::PointingHandCursor);
        connect(dot, &QPushButton::clicked, [this, i](){ jumpToImage(i); }); // 连接信号与槽
        dots.append(dot);
    }

    // 布局设置
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *imageLayout = new QHBoxLayout();
    imageLayout->addWidget(prevButton);
    imageLayout->addWidget(imageLabel);
    imageLayout->addWidget(nextButton);
    mainLayout->addLayout(imageLayout);

    // 点的布局
    QHBoxLayout *dotsLayout = new QHBoxLayout();
    dotsLayout->addStretch();
    dotsLayout->setSpacing(5); // 设置点之间的间距
    for (QPushButton *dot : dots) {
        dotsLayout->addWidget(dot);
    }
    dotsLayout->addStretch();
    mainLayout->addLayout(dotsLayout);

    // 设置定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Carousel::nextImage);
    timer->start(4500); // 3秒切换一次图片

    // 更新点的状态
    updateDots();
}

void Carousel::nextImage() {
    currentImageIndex = (currentImageIndex + 1) % images.size();
    imageLabel->setPixmap(images[currentImageIndex].scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    updateDots();
}

void Carousel::prevImage() {
    currentImageIndex = (currentImageIndex - 1 + images.size()) % images.size();
    imageLabel->setPixmap(images[currentImageIndex].scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    updateDots();
}

void Carousel::jumpToImage(int index) {
    currentImageIndex = index;
    imageLabel->setPixmap(images[currentImageIndex].scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    updateDots();
}

void Carousel::updateDots() {
    for (int i = 0; i < dots.size(); ++i) {
        if (i == currentImageIndex) {
            // 选中状态
            dots[i]->setStyleSheet("background-color: black; border-radius: 4px;");
        } else {
            // 未选中状态
            dots[i]->setStyleSheet("background-color: gray; border-radius: 4px;");
        }
    }
}
