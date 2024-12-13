#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tcpsocket.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    //隐藏原标题栏
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

    m_titleBar = new TitleBar(this);
    connect(m_titleBar, &TitleBar::minimizeClicked, this, &MainWindow::showMinimized);
    connect(m_titleBar, &TitleBar::maximizeClicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });
    connect(m_titleBar, &TitleBar::closeClicked, this, &MainWindow::close);
    connect(m_titleBar, &TitleBar::dragStarted, this, &MainWindow::startDragging);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_titleBar);
    mainLayout->addStretch();

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(mainLayout);
    setCentralWidget(centralWidget);

    TcpSocket *socket = new TcpSocket;

    socket->connectToHost("127.0.0.1", 8284);

    // if (!socket->waitForConnected(3000)) {
    //     qDebug() << "Failed to connect to server";
    // }

    QThread::sleep(2); // 等待2秒

    // 发送消息到服务器
    QByteArray message = "Hello, Server!";
    socket->sendMessageToServer(message);

    // 发送另一条消息
    QByteArray anotherMessage = "Another message.";
    socket->sendMessageToServer(anotherMessage);

    delete socket;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startDragging(const QPoint &globalPos)
{
    m_dragging = true;
    m_dragPosition = globalPos - frameGeometry().topLeft();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        move(event->globalPosition().toPoint() - m_dragPosition);
        event->accept();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
        event->accept();
    }
}
