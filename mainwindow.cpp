#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    AIFile = new SSAIReader("D:/Lydia/c++/AI/Test.ai");
    AIFile->ReadAIFile();

}

MainWindow::~MainWindow()
{
    delete ui;
}
