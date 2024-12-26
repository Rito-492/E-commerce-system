#include "searchline.h"
#include "ui_searchline.h"

SearchLine::SearchLine(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SearchLine)
{
    ui->setupUi(this);

    ui->possibleResultsListWidget->hide();

    connect(ui->searchLineEdit, &QLineEdit::returnPressed, this, [this]() {
        emit returnPressed();
    });
}

SearchLine::~SearchLine()
{
    delete ui;
}

void SearchLine::on_searchButton_clicked()
{
    this->raise();

    ui->possibleResultsListWidget->show();
}

