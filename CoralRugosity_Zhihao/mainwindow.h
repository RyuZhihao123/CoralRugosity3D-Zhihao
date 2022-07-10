#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"

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

protected:

    void resizeEvent(QResizeEvent* e);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
