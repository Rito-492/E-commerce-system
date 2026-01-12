#ifndef SEARCHLINE_H
#define SEARCHLINE_H

#include <QLineEdit>
#include <QListWidget>
#include <QWidget>

namespace Ui {
class SearchLine;
}

class SearchLine : public QWidget
{
    Q_OBJECT

public:
    explicit SearchLine(QWidget *parent = nullptr);
    ~SearchLine();

signals:
    void returnPressed();

private slots:
    void on_searchButton_clicked();

private:
    Ui::SearchLine *ui;
};

#endif // SEARCHLINE_H
