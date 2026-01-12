#include <QtCharts>
#include <QApplication>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QMainWindow window;

    // 创建数据集
    QBarSet *set0 = new QBarSet("Jane");
    QBarSet *set1 = new QBarSet("John");

    *set0 << 1 << 2 << 3 << 4 << 5 << 6;
    *set1 << 5 << 0 << 0 << 4 << 0 << 7;

    // 创建柱状图系列
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    series->append(set1);

    // 创建图表并设置系列
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Simple barchart example");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // 创建分类轴
    QStringList categories;
    categories << "Jan" << "Feb" << "Mar" << "Apr" << "May" << "Jun";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0, 15);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // 创建图表视图并设置图表
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // 设置窗口的主部件
    window.setCentralWidget(chartView);
    window.resize(800, 600);
    window.show();

    return a.exec();
}
