#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "SSAIReader.h"
#include <QMainWindow>
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    SSAIReader* AIFile;

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
