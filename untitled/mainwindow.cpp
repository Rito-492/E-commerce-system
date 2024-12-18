#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    messageList = new QListWidget(this);
    messageList->setViewMode(QListView::IconMode);
    messageList->setResizeMode(QListView::Adjust);
    messageList->setSpacing(10);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(messageList);
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // 添加示例消息
    addMessage("Hello, this is a bubble message!");
    addMessage("Another message here...");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addMessage(const QString &text)
{
    BubbleMessageWidget *bubble = new BubbleMessageWidget(text);
    QListWidgetItem *item = new QListWidgetItem;
    messageList->setItemWidget(item, bubble);
    messageList->addItem(item);
    messageList->scrollToBottom();
}
