// Local
#include "MainWindow.h"
#include "ui_mainwindow.h"
#include "WriterCdWidget.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , m_writerCdWidget(new WriterCdWidget(""))
{
  ui->setupUi(this);
  setCentralWidget(m_writerCdWidget);
}

MainWindow::~MainWindow()
{
  delete ui;
}
