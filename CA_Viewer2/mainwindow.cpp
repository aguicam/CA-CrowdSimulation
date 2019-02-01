#include <QFileDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>


MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_action_Quit_triggered()
{
	 QApplication::quit();
}

void MainWindow::on_checkBoxFill_toggled(bool checked)
{
	 ui->openGLWidget->setPolygonMode(checked);
}

void MainWindow::on_action_Open_triggered()
{
	QString filename = QFileDialog::getOpenFileName(this, tr("Open PLY"), ".", tr("*.ply"));

	ui->openGLWidget->loadMesh(filename);
}

void MainWindow::on_resetButton_clicked()
{
    ui->openGLWidget->resetScene();
}



void MainWindow::on_pushButton_clicked()
{
    ui->openGLWidget->changeStop();
}


void MainWindow::on_showAstar_valueChanged(int arg1)
{
    ui->openGLWidget->setShowPathAgent(arg1);
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    ui->openGLWidget->setAstar();
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    //ui->openGLWidget->setAstar();
}
