#ifndef BUBBLEMESSAGEWIDGET_H
#define BUBBLEMESSAGEWIDGET_H

#include <QWidget>
#include <QString>

class BubbleMessageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BubbleMessageWidget(const QString &text, QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QString messageText;
};

#endif // BUBBLEMESSAGEWIDGET_H
