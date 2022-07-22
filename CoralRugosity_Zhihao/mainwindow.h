#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QtCharts>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    GLWidget* m_glwidget;
    QChart * m_chart_crosssection;
    QChart * m_chart_height;
protected:

    void resizeEvent(QResizeEvent* e);

protected slots:
    void slot_updateChart(float minY, float maxY);
    void slot_updateStatus();

    void slot_saveRugosity();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
