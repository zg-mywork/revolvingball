#include "nkview.h"
#include <iostream>
NKView::NKView(QWidget *parent) :
    QTreeView(parent)
{
    nkMode=new QStandardItemModel;
    connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(IndexToNode(QModelIndex)));
    connect(this,SIGNAL(activated(QModelIndex)),this,SLOT(IndexToNode(QModelIndex)));
    //connect(this,SIGNAL(entered(QModelIndex)),this,SLOT(IndexToNode(QModelIndex)));
}

void NKView::Traverse(PREGTREENODE regtreenode)
{

    this->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
   // nkMode->setHorizontalHeaderLabels(QStringList()<<"");
    this->setHeaderHidden(true);
    QStandardItem *treeItem=new QStandardItem(regtreenode->name);
    if(regtreenode->isdeleted)
        treeItem->setForeground(QBrush(QColor(200,0,0)));
    nkMode->appendRow(treeItem);
    do_traverse(regtreenode,treeItem);
    this->setFocus();
    this->setModel(nkMode);

}

void NKView::IndexToNode(QModelIndex i)
{
    emit currentNode(Mapregnode[i]);
}

void NKView::nk_unload()
{
    nkMode->clear();
    //std::cout<<"nkunload"<<std::endl;
}
void NKView::do_traverse(PREGTREENODE regnode, QStandardItem *regItem)
{
   Mapregnode[regItem->index()]=regnode;
   for(PREGTREENODE &node:regnode->child){
       QStandardItem *childItem=new QStandardItem(node->name);
       regItem->appendRow(childItem);
       do_traverse(node,childItem);
   }
}
