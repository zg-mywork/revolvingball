#ifndef NKVIEW_H
#define NKVIEW_H

#include <QTreeView>
#include "treestructure.h"
#include<QStandardItem>
#include<QStringList>
#include<QStandardItemModel>
#include<map>
class NKView : public QTreeView
{
    Q_OBJECT
public:
    explicit NKView(QWidget *parent = 0);
    ~NKView(){delete nkMode;

             }
    void Traverse(PREGTREENODE regtreenode);

signals:
 void currentNode(PREGTREENODE node);

public slots:
 void IndexToNode(QModelIndex i);
 void nk_unload();
private:
    QStandardItemModel *nkMode;
    std::map<QModelIndex,PREGTREENODE> Mapregnode;
private:
    void do_traverse(PREGTREENODE regnode,QStandardItem *regItem);
};

#endif // NKVIEW_H
