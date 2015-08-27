#include "mainwindow.h"
#include "QCoreApplication"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    display();

    connect(NKview,SIGNAL(currentNode(PREGTREENODE)),VKview,SLOT(showVK(PREGTREENODE)));
    connect(this,SIGNAL(signal_unload()),NKview,SLOT(nk_unload()));
    connect(this,SIGNAL(signal_unload()),VKview,SLOT(vk_unload()));
}

MainWindow::~MainWindow()
{
    for(registry* r:reg){
                delete r;
        }
    reg.clear();
}

void MainWindow::display()
{

    barlabel=new QLabel(this);
    barlabel->setText("Ready");
    statusBar =new QStatusBar(this);
    statusBar->addWidget(barlabel);
    this->setStatusBar(statusBar);
    //statusBar->showMessage("Ready");
    //statusBar->->addWidget(new MyReadWriteIndication(statusBar()));
    QWidget *centralWidget=new QWidget(this);
    this->setCentralWidget(centralWidget);

    NKview = new NKView(centralWidget);
    VKview = new VKView(centralWidget);
    this->resize(800,600);
    menu =new QMenuBar(this);
    QMenu *filemenu=new QMenu("File",this);
    filemenu->addAction("Load hive",this,SLOT(loadHive()),Qt::CTRL+Qt::Key_O);
    filemenu->addAction("Load hive-deleted",this,SLOT(loadDeletedHive()),0);
    filemenu->addAction("Unload all",this,SLOT(unload()),Qt::CTRL+Qt::Key_U);
    filemenu->addAction("Exit",this,SLOT(close()),0);

    QMenu *helpmenu=new QMenu("Help",this);
    helpmenu->addAction("About AnyviewReg",this,SLOT(showAbout()),0);

    menu->addMenu(filemenu);
    menu->addMenu(helpmenu);
    this->setMenuBar(menu);

     QHBoxLayout *mainlayout=new QHBoxLayout(centralWidget);
     QSplitter *splitter = new QSplitter(centralWidget);

     splitter->addWidget(NKview);
     splitter->addWidget(VKview);
     splitter->setStretchFactor(0,2);
     splitter->setStretchFactor(1,7);
     mainlayout->addWidget(splitter);
}

void MainWindow::unload()
{
  emit signal_unload();
    for(registry* r:reg){
                delete r;
        }
    reg.clear();
}

void MainWindow::showAbout()
{
   QString msg="Version: 1.0";
   msg.append("\n").\
           append("Author: ZhaoGang").\
           append("\n").append("Develop Tool: QT").\
           append("\n").append("Time: Sep 15th , 2014");
   //QMessageBox::information(this, "About", msg, QMessageBox::Ok);
   QMessageBox::about(this,"About",msg);
}

void MainWindow::loadHive()
{

    QString fileName;
    QString currentpath;
    currentpath=QCoreApplication::applicationDirPath();

    if((fileName = QFileDialog::getOpenFileName(this,
        tr("Open Hive"), currentpath, "All Files (*.*)"))=="")
        return;
    statusBar->showMessage("please wait……",2000);
    string ms=fileName.toStdString();

     const char *path=ms.c_str();
     registry *r=new registry();
     reg.push_back(r);
     if(!r->connect(path)){
         QMessageBox::warning(this, "error","invalid hive file!" , QMessageBox::Ok);
          return ;
     }
     r->analysis();
     this->NKview->Traverse(r->getRetRoot());
}

void MainWindow::loadDeletedHive()
{
    QString fileName;
    QString currentpath;
    currentpath=QCoreApplication::applicationDirPath();
    if((fileName = QFileDialog::getOpenFileName(this,
        tr("Open Hive"), currentpath, "All Files (*.*)"))=="")
        return;
    statusBar->showMessage("please wait……",2000);
    string ms=fileName.toStdString();

     const char *path=ms.c_str();
     registry *r=new registry();
     reg.push_back(r);
     if(!r->connect(path)){
         QMessageBox::warning(this, "error","invalid hive file!" , QMessageBox::Ok);
          return ;
     }
     r->analysisDeleted();
    this->NKview->Traverse(r->getRetRoot());
}
