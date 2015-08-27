#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<QMenuBar>
#include<QMainWindow>
#include"nkview.h"
#include"vkview.h"
#include<QLayout>
#include<QString>
#include<QFileDialog>
#include<iostream>
#include"registry.h"
#include<vector>
#include<QMessageBox>
#include<QStatusBar>
#include<QSplitter>
#include<QLabel>
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void signal_unload();
public slots:
    void loadHive();
    void loadDeletedHive();
    void unload();
    void showAbout();
private:
    void display();
private:
    QMenuBar *menu;
    NKView *NKview;
    VKView  *VKview;
    QStatusBar *statusBar;
    QLabel *barlabel;
    std::vector<registry*> reg;
    //QString fileName;
};

#endif // MAINWINDOW_H
