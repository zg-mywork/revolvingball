#include "vkview.h"
#include "iostream"
VKView::VKView(QWidget *parent) :
    QTableView(parent)
{

   vkMode=new QStandardItemModel;
   display();

}

VKView::~VKView()
{
    delete vkMode;
    //vkMode->clear();
}

void VKView::showVK(PREGTREENODE regnode)
{

    vkMode->clear();
    if(regnode->isdeleted==false){
        display();
        //vkMode->removeRows(1,vkMode->rowCount());
        for(PVALUE value:regnode->value){
            //qs<<QStandardItem(value->Vname)<<QStandardItem(value->Vtype)<<QStandardItem(value->Vdata);
            QStandardItem *name=new QStandardItem(value->Vname);
            QStandardItem *type=new QStandardItem(value->Vtype);
            QStandardItem *data=new QStandardItem(value->Vdata);
            //data->setFont(QFont::Black);
            vkMode->appendRow(QList<QStandardItem*>()<<name<<type<<data);
        }
    }else{
        displayDeleted();
        for(PVALUE value:regnode->value){

            QStandardItem *name=new QStandardItem(value->Vname);
            QStandardItem *type=new QStandardItem(value->Vtype);
            QStandardItem *dir=new QStandardItem(value->dir);
            QStandardItem *data=new QStandardItem(value->Vdata);
            QStandardItem *time=new QStandardItem(value->time);
            vkMode->appendRow(QList<QStandardItem*>()<<type<<name<<dir<<data<<time);
        }

    }
    this->setModel(vkMode);
}

void VKView::vk_unload()
{
    std::cout<<"vkunload"<<std::endl;
    vkMode->clear();

    display();
}

void VKView::display()
{
    vkMode->setHorizontalHeaderLabels(QStringList()<<"Name"<<"Type"<<"Data");
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->resizeRowsToContents();
    this->setModel(vkMode);
    this->setColumnWidth(0,200);
    this->setColumnWidth(2,300);
}

void VKView::displayDeleted()
{
    vkMode->setHorizontalHeaderLabels(QStringList()<<"Type"<<"Name"<<"Dir"<<"Data"<<"Time");
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->resizeRowsToContents();
    this->setModel(vkMode);
   // this->setColumnWidth(0,200);
    //this->setColumnWidth(2,300);
}
