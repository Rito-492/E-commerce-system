// // main.cpp
// #include <QApplication>
// #include <QWidget>
// #include <QStackedWidget>
// #include <QPropertyAnimation>
// #include <QVBoxLayout>
// #include <QTimer>
// #include <QLabel>
// #include <QPixmap>

// class Carousel : public QWidget {
//     Q_OBJECT

// public:
//     Carousel(const QStringList &imagePaths, QWidget *parent = nullptr)
//         : QWidget(parent), currentIndex(0), imagePaths(imagePaths) {

//         setFixedSize(400, 300); // 固定窗口大小

//         stackedWidget = new QStackedWidget(this);

//         // 添加图片到QStackedWidget
//         for (const QString &path : imagePaths) {
//             QLabel *label = new QLabel;
//             label->setPixmap(QPixmap(path).scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
//             label->setAlignment(Qt::AlignCenter);
//             stackedWidget->addWidget(label);
//         }

//         QVBoxLayout *layout = new QVBoxLayout(this);
//         layout->addWidget(stackedWidget);

//         // 定时器用于自动切换页面
//         QTimer *timer = new QTimer(this);
//         connect(timer, &QTimer::timeout, this, &Carousel::nextPage);
//         timer->start(3000); // 每3秒切换一次

//         // 初始化动画
//         animation = new QPropertyAnimation(stackedWidget, "pos");
//         animation->setDuration(1000); // 动画持续时间1秒
//     }

// public slots:
//     void nextPage() {
//         int nextIndex = (currentIndex + 1) % imagePaths.size();

//         // 设置动画的开始和结束位置
//         animation->setStartValue(QPoint(0, 0));
//         animation->setEndValue(QPoint(-width(), 0));

//         // 在动画结束时切换页面
//         connect(animation, &QPropertyAnimation::finished, this, [this, nextIndex]() {
//             stackedWidget->setCurrentIndex(nextIndex);
//             stackedWidget->move(0, 0); // 重置位置
//             currentIndex = nextIndex;
//         });

//         animation->start();
//     }

// private:
//     QStackedWidget *stackedWidget;
//     QPropertyAnimation *animation;
//     QStringList imagePaths;
//     int currentIndex;
// };

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);

//     // 图片路径列表
//     QStringList imagePaths = {
//         ":/images/image1.jpg",
//         ":/images/image2.jpg",
//         ":/images/image3.jpg"
//     };

//     Carousel carousel(imagePaths);
//     carousel.show();

//     return app.exec();
// }

// #include "main.moc"

#include <QApplication>
#include "carousel.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QVector<QString> imagePaths = {
        ":/images/image1.jpg",
        ":/images/image2.jpg",
        ":/images/image3.jpg",
        ":/images/image4.jpg",
        ":/images/image5.jpg",
        ":/images/image6.jpg"
        // 更多图片路径...
    };

    Carousel *carousel = new Carousel(imagePaths);
    carousel->show();

    return a.exec();
}
