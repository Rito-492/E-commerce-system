#include "titlebar.h"

#include <QHBoxLayout>

TitleBar::TitleBar(QWidget *parent) : QWidget(parent)
{
    //创建按钮插入图片
    themeButton = new QPushButton(QIcon(":/prefix1/icons/theme.png"), "", this);
    minimizeButton = new QPushButton(QIcon(":/prefix1/icons/minimize.png"), "", this);
    maximizeButton = new QPushButton(QIcon(":/prefix1/icons/maximize.png"), "", this);
    closeButton = new QPushButton(QIcon(":/prefix1/icons/close.png"), "", this);

    //设置按钮无边框并设置大小
    themeButton->setStyleSheet("border: none;"); themeButton->setFixedSize(30, 30);
    minimizeButton->setStyleSheet("border: none;"); minimizeButton->setFixedSize(30, 30);
    maximizeButton->setStyleSheet("border: none;"); maximizeButton->setFixedSize(30, 30);
    closeButton->setStyleSheet("border: none;"); closeButton->setFixedSize(30, 30);

    QHBoxLayout *layout = new QHBoxLayout(this);

    // 添加伸缩项， 把按钮推到最右边
    layout->addStretch();
    layout->addWidget(themeButton);
    layout->addWidget(minimizeButton);
    layout->addWidget(maximizeButton);
    layout->addWidget(closeButton);

    // 移除布局的边距
    layout->setContentsMargins(0, 0, 0, 0);

    // 移除按钮之间的间距
    layout->setSpacing(0);

    // 连接按钮点击槽函数
    connect(minimizeButton, &QPushButton::clicked, this, &TitleBar::minimizeClicked);
    connect(maximizeButton, &QPushButton::clicked, this, &TitleBar::maximizeClicked);
    connect(closeButton, &QPushButton::clicked, this, &TitleBar::closeClicked);
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        emit dragStarted(event->globalPosition().toPoint());
        event->accept();
    }
}
