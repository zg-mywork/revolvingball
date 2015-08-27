#ifndef VKVIEW_H
#define VKVIEW_H

#include <QTableView>
#include <QStandardItemModel>
#include"treestructure.h"
#include<vector>
class VKView : public QTableView
{
    Q_OBJECT
public:
    explicit VKView(QWidget *parent = 0);
    ~VKView();

signals:

public slots:
    void showVK(PREGTREENODE regnode);
    void vk_unload();
    //void showDeleted(std::vector<PREGDELETED> pd);
private:
     QStandardItemModel *vkMode;
     void display();
     void displayDeleted();
     //void showDeleted();
};

#endif // VKVIEW_H
