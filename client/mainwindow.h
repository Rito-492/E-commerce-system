#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "titlebar.h"

#include <QMainWindow>
#include <QMessageBox>
#include <QMouseEvent>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void startDragging(const QPoint &globalPos);

private:
    Ui::MainWindow *ui;
    TitleBar *m_titleBar;
    bool m_dragging = false;
    QPoint m_dragPosition;

};

#endif // MAINWINDOW_H
