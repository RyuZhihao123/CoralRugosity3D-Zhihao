#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "3rdparty/happly.h"
#include "3rdparty/tinyply.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_glwidget = new GLWidget(ui->centralWidget);
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    qDebug()<<"resize()";
    ui->groupBox->move(ui->centralWidget->width()-ui->groupBox->width(),0);

    m_glwidget->move(0,0);
    m_glwidget->resize(ui->centralWidget->width()-ui->groupBox->width()-5,ui->centralWidget->height());
}

MainWindow::~MainWindow()
{
    delete ui;
}
