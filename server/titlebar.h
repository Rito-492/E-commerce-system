#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPushButton>
#include <QWidget>

class TitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit TitleBar(QWidget *parent = nullptr);

signals:
    void minimizeClicked();
    void maximizeClicked();
    void closeClicked();
    void dragStarted(const QPoint &globalPos);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QPushButton *themeButton;
    QPushButton *minimizeButton;
    QPushButton *maximizeButton;
    QPushButton *closeButton;
};

#endif // TITLEBAR_H
