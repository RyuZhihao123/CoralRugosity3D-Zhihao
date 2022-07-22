#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utility/happly.h"
#include "utility/tinyply.h"
#include <QDebug>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_glwidget = new GLWidget(ui->centralWidget);

    // initialize UI
    ui->ckbWireFrame->setChecked(m_glwidget->m_isWireFrame);
    ui->ckbBVHS->setChecked(m_glwidget->m_isShowBVH);

    connect(ui->ckbWireFrame,SIGNAL(clicked(bool)), this, SLOT(slot_updateStatus()));
    connect(ui->ckbBVHS,SIGNAL(clicked(bool)), this, SLOT(slot_updateStatus()));
    connect(ui->btnSaveRugosity,SIGNAL(clicked(bool)),this,SLOT(slot_saveRugosity()));

    // Initialize Chart
    m_chart_crosssection = new QChart();
    m_chart_crosssection->legend()->hide();

    QLineSeries* series = new QLineSeries();
    series->append(0,0);
    series->append(10,0);
    m_chart_crosssection->setTitle("Rugosity - CrossSection");

    m_chart_crosssection->setTheme(QChart::ChartThemeBlueCerulean);
    m_chart_crosssection->setFont(QFont(QString("微软雅黑"),15,2));
    m_chart_crosssection->setTitleFont(QFont(QString("微软雅黑"),10,2));
    m_chart_crosssection->addSeries(series);
    m_chart_crosssection->createDefaultAxes();
    ui->graphicsView->setChart(m_chart_crosssection);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

    // Initialize Chart 2
    m_chart_height = new QChart();
    m_chart_height->legend()->hide();

    QLineSeries* series_2 = new QLineSeries();
    series_2->append(0,0);
    series_2->append(10,0);
    m_chart_height->setTitle("Rugosity - Height");

    m_chart_height->setTheme(QChart::ChartThemeBlueCerulean);
    m_chart_height->setFont(QFont(QString("微软雅黑"),15,2));
    m_chart_height->setTitleFont(QFont(QString("微软雅黑"),10,2));
    m_chart_height->addSeries(series_2);
    m_chart_height->createDefaultAxes();
    ui->graphicsView_2->setChart(m_chart_height);
    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);

    connect(m_glwidget, SIGNAL(sig_updateChart(float, float)), this, SLOT(slot_updateChart(float, float)));
}

void MainWindow::slot_updateChart(float minY, float maxY)
{
    // Update Cross-section Chart
    m_chart_crosssection->removeAllSeries();
    QVector<QVector<QVector2D>> curves = m_glwidget->m_2D_curves;

    QVector<QLineSeries*> series;
    series.resize(curves.size());
    for(int i=0; i<curves.size(); ++i)
    {
        series[i] = new QLineSeries();
        QVector<QVector2D>& curCurve = curves[i];
        for(int p=0; p<curCurve.size(); ++p)
        {
            series[i]->append(curCurve[p].toPointF());
        }

        m_chart_crosssection->addSeries((series[i]));
        QPen pen = series[i]->pen();
        pen.setWidth(5);
        series[i]->setPen(pen);
    }

    m_chart_crosssection->createDefaultAxes();


    // Update Height Chart
    m_chart_height->removeAllSeries();
    QVector<QVector2D> heights = m_glwidget->m_2D_heights;

    QLineSeries* series_height = new QLineSeries();
    for(int p=0; p<heights.size(); ++p)
    {
        series_height->append(heights[p].toPointF());
    }

    QPen pen = series_height->pen();
    pen.setWidth(5);
    pen.setColor(QColor(255,0,0));
    series_height->setPen(pen);

    m_chart_height->addSeries(series_height);
    QPen pen_height = series_height->pen();
    pen_height.setWidth(5);
    series_height->setPen(pen_height);


    m_chart_height->createDefaultAxes();
    m_chart_height->axisY()->setRange(minY, maxY);
    m_chart_crosssection->axisY()->setRange(minY, maxY);
    //    qDebug()<<(m_chart_crosssection->axes().size());

}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    qDebug()<<"resize()";
    ui->groupBox->move(ui->centralWidget->width()-ui->groupBox->width(),0);

    m_glwidget->move(0,0);
    m_glwidget->resize(ui->centralWidget->width()-ui->groupBox->width()-5,ui->centralWidget->height()*0.6);
    ui->graphicsView->move(0, m_glwidget->height());

    float newchartHeight = ui->centralWidget->height() * (1.0 - m_glwidget->height()/(float)height());
    float chartRatio = newchartHeight/ui->graphicsView->height();
    float newchartWidth = ui->graphicsView->width() * chartRatio;
    ui->graphicsView->resize(newchartWidth, newchartHeight);

    ui->graphicsView_2->move(ui->graphicsView->width(), m_glwidget->height());
    ui->graphicsView_2->resize(newchartWidth, newchartHeight);
}

void MainWindow::slot_updateStatus()
{
    m_glwidget->m_isWireFrame = ui->ckbWireFrame->isChecked();
    m_glwidget->m_isShowBVH = ui->ckbBVHS->isChecked();
    m_glwidget->update();
}

void MainWindow::slot_saveRugosity()
{
    QString filepath = QFileDialog::getSaveFileName(this,tr("Save Rugosity File"), ".", tr("Rugosity (*.xls)"));

    qDebug()<<filepath;

    if(filepath == "")
        return;

    QFileInfo fileinfo(filepath);

    qDebug()<<fileinfo.absolutePath();
    qDebug()<<fileinfo.baseName();
    qDebug()<<fileinfo.suffix();

    {
        // 首先存档Rugosity的height
        QString filename_height = fileinfo.absolutePath() + "/" + fileinfo.baseName() + "_height."+fileinfo.suffix();

        QFile file(filename_height);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream ts(&file);

        ts<<"x"<<"\t"<<"y"<<"\t\n";

        for(int i=0; i<m_glwidget->m_2D_heights.size(); ++i)
        {
            ts<<m_glwidget->m_2D_heights[i].x()<<"\t"<<m_glwidget->m_2D_heights[i].y()<<"\t\n";
        }

        file.close();
        qDebug()<<"保存height完毕!";
    }

    {
        // 首先存档Rugosity的height
        QString filename_crosssection = fileinfo.absolutePath() + "/" + fileinfo.baseName() + "_crossSection."+fileinfo.suffix();

        QFile file(filename_crosssection);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return;

        QTextStream ts(&file);

        int max_point_size = 0;

        auto& curves2D = m_glwidget->m_2D_curves;
        auto& curves3D = m_glwidget->m_rugosity->m_curves;

        for(int i=0; i<curves3D.size(); ++i)
        {
            if(curves3D[i].pts.size() > max_point_size)
                max_point_size = curves3D[i].pts.size();
        }

        for(int cid = 0; cid<curves2D.size(); ++cid)
        {
            ts<<"X-2d"<<"\t"<<"Y-2d"<<"\t"<<"X-3d"<<"\t"<<"Y-3d"<<"\t"<<"Z-3d"<<"\t\t";

        }
        ts<<"\n";

        for(int i=0; i<max_point_size; ++i)  // 第i行
        {
            for(int cid = 0; cid<curves2D.size(); ++cid)  // 第cid个2D曲线
            {

                if(i >= curves2D[cid].size())
                {
                    ts<<""<<"\t"<<""<<"\t";
                }
                else
                {
                    ts<<curves2D[cid][i].x()<<"\t"<<curves2D[cid][i].y()<<"\t";
                }

                if(i >= curves3D[cid].pts.size())
                {
                    ts<<""<<"\t"<<""<<"\t"<<""<<"\t";
                }
                else
                {
                    ts<<curves3D[cid].pts[i].x()<<"\t"<<curves3D[cid].pts[i].y()<<"\t"<<curves3D[cid].pts[i].z()<<"\t";
                }

                ts<<"\t";
            }

            ts<<"\n";
        }



        file.close();
        qDebug()<<"保存crosssection完毕!";
    }

    //    {

    //        QFile file(filepath);
    //        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    //            return;

    //        QTextStream ts(&file);

    //        ts<<"2D curves"<<"\n";

    //        QVector<Curve> curves3D = m_glwidget->m_rugosity->m_curves ;


    //        // 首先计算最大的点的数目
    //        int max = 0;
    //        for(int i=0; i<curves2D.size(); ++i)
    //        {
    //            if(max < curves2D[i].size())
    //            {
    //                max = curves2D[i].size();
    //            }
    //        }
    //    }


}

MainWindow::~MainWindow()
{
    delete ui;
}
