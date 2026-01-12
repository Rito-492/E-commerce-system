#include "BubbleMessageWidget.h"
#include <QPainter>
#include <QPalette>
#include <QBrush>
#include <QFontMetrics>

BubbleMessageWidget::BubbleMessageWidget(const QString &text, QWidget *parent)
    : QWidget(parent), messageText(text)
{
    setFixedSize(200, 100); // 你可以根据需要调整大小
    setStyleSheet("background-color: lightblue; border-radius: 10px; padding: 10px;");
}

void BubbleMessageWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 这里你可以添加绘制气泡尾巴的代码（三角形或其他形状）
    // 但为了简化，我们直接使用圆角矩形作为示例

    // 绘制文本
    painter.setFont(font());
    painter.setPen(Qt::black);
    painter.drawText(rect(), Qt::AlignLeft | Qt::AlignTop, messageText);
}
