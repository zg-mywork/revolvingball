#include "registry.h"
#include<QString>
registry::registry()
{

}

registry::registry(char *fname)
{
    if(freg.is_open())
        freg.close();
    freg.open(fname,ios_base::binary|ios_base::in);
    if(freg.is_open()){
        freg.seekg(0,ios_base::beg);
        auto beg=freg.tellg();
        freg.seekg(0,ios_base::end);
        auto size=freg.tellg()-beg;
        freg.seekg(0,ios_base::beg);
        buffer =new u8[size];
        freg.read(buffer,size);
        PHEAD head=(PHEAD)buffer;

        phbin_first=buffer+0x1000;
        phbin_last=gopoint(head->offset_hbin_last);
        pnode_tree=gopoint(head->offset_nk_root);
    }
}

registry::~registry()
{
    if(freg.is_open()){

        freg.close();
        delete buffer;
        delete regRoot;
    }
  //  if()
   /* delete phbin_first;
    delete pnode_tree;
    delete phbin_last;*/

}

bool registry::connect(const char *fname)
{
    if(freg.is_open())
        freg.close();
    //fname="C:\\Windows\\System32\\config\\software";
    freg.open(fname,ios_base::binary|ios_base::in);
    if(freg.is_open()){
        freg.seekg(0,ios_base::beg);
        auto beg=freg.tellg();
        freg.seekg(0,ios_base::end);
        auto size=freg.tellg()-beg;
        freg.seekg(0,ios_base::beg);
        buffer =new u8[size];
        freg.read(buffer,size);
        PHEAD head=(PHEAD)buffer;
        if(head->regf!=0x66676572){
            freg.close();
            return false;
        }

        phbin_first=buffer+0x1000;
        phbin_last=gopoint(head->offset_hbin_last);
        pnode_tree=gopoint(head->offset_nk_root);
    }
    return freg.is_open();
}


bool registry::disconnect()
{
    if(freg.is_open()){
        freg.close();
        delete buffer;
    }
    return !freg.is_open();
}

wchar_t *registry::filenameg()
{
    if(freg.is_open()){
        PHEAD head=(PHEAD)buffer;
        return head->name_head;
    }else{
        return NULL;
    }
}

u32 registry::off_root_nkg()
{
    if(freg.is_open()){
        PHEAD head=(PHEAD)buffer;
        return head->offset_nk_root;
    }else{
        return 0;
    }
}

u32 registry::off_hbin_lastg()
{
    if(freg.is_open()){
        PHEAD head=(PHEAD)buffer;
        return head->offset_hbin_last;
    }else{
        return 0;
    }
}

void registry::analysis()
{
    if(freg.is_open()){
         traverseRegtree();
    }else{
        ////cout<<"file is closed!"<<endl;
    }
}

void registry::analysisDeleted()
{
    regRoot =new REGTREENODE;
    regRoot->isdeleted=true;
    regRoot->name="deleted-"+getHeaderName();
    u8 *p=phbin_first;
    while(p<phbin_last){
        if(*(u32*)p==0x6e696268){
            p+=0x20;
            /*cout<<(u32*)p<<hex<<endl;
            cout<<(u32*)phbin_first<<hex<<endl;
            cout<<hex<<"spf: "<<(u32)p-(u32)phbin_first<<endl;
            cout<<hex<<"sp: "<<(u32)p-(u32)phbin_first+0x1000<<endl;*/
        }else{
            if(*(int*)p<0){
               /* cout<<hex<<"1: "<<(u32)p-(u32)phbin_first+0x1000<<endl;
                cout<<"p1: "<<(*(int*)p)<<endl;*/
                p-=(*(int*)p);
               // cout<<hex<<"2: "<<(u32)p-(u32)phbin_first+0x1000<<endl;
            }else{
               /* cout<<"point: "<<hex<<(u32)p-(u32)phbin_first+0x1000<<endl;
                cout<<"size: "<<(*(int*)p)<<endl;
                cout<<hex<<"32 :  "<<(*(u32*)p)<<endl;
                cout<<hex<<"p : "<<(u32)p<<endl;*/
               // cout<<hex<<"phbin_last : "<<(u32)phbin_last<<endl;
               /* if(0x7a1938==(u32)p-(u32)phbin_first+0x1000)
                {
                    int i;
                    i++;
                }*/
                do_AD(p);
                p+=(*(int*)p);
            }
        }
    }
}

u8 *registry::gopoint(u32 offset)
{
    if(freg.is_open()){

        return phbin_first+offset;
    }else{
        return NULL;
    }
}

QString registry::getHeaderName()
{
    if(freg.is_open()){
        PHEAD head=(PHEAD)buffer;
        wstring name((const wchar_t*)head->name_head,0x20);
        QString headname=QString::fromStdWString(name);
        return headname;
    }
    return NULL;
}

QString registry::getNKname(PNK pnk)
{
    QString name;
    if(pnk->type_nk==0x00){//nk name use different code with different type
        wstring nameNK((const wchar_t*)pnk->name_nk,pnk->length_name_nk/2);
        name=QString::fromStdWString(nameNK);
    }else{
        string nameNK;
        nameNK.assign((const char*)pnk->name_nk,pnk->length_name_nk);
        name=QString::fromStdString(nameNK);
    }
    return name;
}

QString registry::getVKname(PVK pvk)
{
    QString vkname;
    int size=-pvk->size_vk;
    if(size<0)
        size=-size;
    if(size==0x18){//no name of "Default" is stored in file
        vkname="(Default)";
    }else if(pvk->is_used==0x00){
        wstring Vname((wchar_t*)pvk->name_vk,pvk->length_name_vk/2);
        vkname=QString::fromStdWString(Vname);
    }else if(size<0x18){
        vkname="error";
    }else{
        string Vname(pvk->name_vk,pvk->length_name_vk);
        vkname=QString::fromStdString(Vname);
    }
    return vkname;
}

int registry::lenthVK_deleted(u8 *p)
{
    PVK pvk=(PVK)p;
    u32 len;
    len=((pvk->length_name_vk%0x08==0)?pvk->length_name_vk:((pvk->length_name_vk/8+1)*8))+0x18;
    //u32 t=((pvk->length_name_vk%0x08==0)?pvk->length_name_vk:((pvk->length_name_vk/8+1)*8));
    //u32 lvk=pvk->length_name_vk;
    //cout<<"VKLen_deleted  "<<len<<endl;

    return len;
}

int registry::lenthNK_deleted(u8 *p)
{
    PNK pnk=(PNK)p;
    int len;
    len=((pnk->length_name_nk%8==0)?pnk->length_name_nk:((pnk->length_name_nk/8+1)*8)+0x50);
    //cout<<"NKLen_deleted  "<<len<<endl;

    return len;
}

void registry::do_AD(u8 *p)
{
    u8 *cp=p;
    u32 size=*(u32*)cp;
    if(size==8)
        return;
    while(cp-p<=size){
        switch(*(u16*)(cp+4)){
        case 0x6b6e:{deletedNK(cp);cp+=lenthNK_deleted(cp);}break;
        case 0x6b76:{deletedVK(cp);cp+=lenthVK_deleted(cp);}break;
        default:return;break;
        }
    }
}
/**
 * @brief registry::isleap is leap year?
 * @param year
 * @return
 */
bool registry::isleap(int year)
{
    if(year%4==0){
        if(year%100==0){
            if(year%400==0)
                return true;
            else
                return false;
        }
        return true;
    }
    return false;
}

QString registry::getTime(unsigned long long * tptr)
{
    tm time;
    unsigned long long days=(*tptr)/10000000/3600/24;
    unsigned long long secs=((*tptr)/10000000)%(3600*24);
    int hours=secs/3600;
    int min=(secs%3600)/60;
    int sec=(secs%3600)%60;
    time.tm_hour=hours;
    time.tm_min=min;
    time.tm_sec=sec;
    time.tm_isdst=0;
    //time.
    int year=1601;
    unsigned long long ldays=days;
    while(true){
        if(isleap(year)){
            if(ldays>=366)
            ldays-=366;
            else
                break;
        }
        else {
            if(ldays>=365){
            ldays-=365;
        }
        else
            break;
        }
        year++;
    }
    time.tm_year=year-1900;
    time.tm_yday=ldays;
    int month=0;
    std::set<int> bm={1,3,5,7,8,10,12};
    while(true){
        if(bm.find(month)!=bm.end()){
            if(ldays>=31){
                ldays-=31;
                //month++;
            }else
                break;

        }else{
            if(month==2){
                if(isleap(year)){
                    if(ldays>=29)
                        ldays-=29;
                    else break;
                }else{
                    if(ldays>=28)
                        ldays-=28;
                    else
                        break;
                }
                //month++;
            }else{
                if(ldays>=30){
                    ldays-=30;
                }else
                    break;
                //month++;
            }
        }
        month++;
    }
    time.tm_mon=month;
    time.tm_mday=ldays+1;
   // time.tm_wday=0;
    time_t t=mktime(&time);
    QString strTime="";
     std::string s;
    if(t==-1){
      return strTime;
    }else{
        s=ctime(&t);
    }
    strTime= QString::fromStdString(s);
    return strTime;
}

void registry::traverseRegtree()
{
  regRoot=new REGTREENODE;
  regRoot->isdeleted=false;
  do_traverse(pnode_tree,regRoot);
}

void registry::do_traverse(u8 *p, PREGTREENODE node)
{

    vector<u32> list_nk;
   /* static int i=0;
    i++;
    if(i>100)
        return;*/
    PNK pnk=(PNK)p;
    node->name=getNKname(pnk);
    node->time=getTime((unsigned long long*)pnk->time_NK);
   // cout<<qPrintable(node->time)<<endl;
    //cout<<"nametype:"<<pnk->type_nk<<endl;
    //cout<<"nk:";
    //cout<<qPrintable(node->name)<<endl;
    // //cout<<pnk->name_nk<<endl;
    getvk(pnk,node,node->name);
    if(pnk->offset_list_child==0xFFFFFFFF)
    {
        //cout<<"no child nk in "<<qPrintable(node->name)<<endl;

    }else{
        u8 *plistchild=gopoint(pnk->offset_list_child);

        PLNKC plnkc=(PLNKC)plistchild;
        u16 num_nk_child=plnkc->num_child;

        switch (plnkc->magic) {
        case 0x666c://lf
            for(int num=0;num!=num_nk_child;num++){
                list_nk.push_back(plnkc->lfh[num].offset_list_child);
            }
            break;
        case 0x686c://lh
            for(int num=0;num!=num_nk_child;num++){
                list_nk.push_back(plnkc->lfh[num].offset_list_child);
            }
            break;
        case 0x6c69://ri
            for(int num=0;num!=num_nk_child;num++){
                list_nk.push_back(plnkc->rli[num].offset_list_child);
            }
            break;
        case 0x7269://li
            for(int num=0;num!=num_nk_child;num++){
                list_nk.push_back(plnkc->rli[num].offset_list_child);
            }
            break;
        default:
            break;
        }
        for(auto poff_child:list_nk){
            u8 *p=gopoint(poff_child);
            PREGTREENODE childnode=new REGTREENODE;
            childnode->isdeleted=false;
            childnode->fatherNK=node;
            //childnode->child=NULL;
            node->child.push_back(childnode);
            do_traverse(p,childnode);
        }

    }

}

void registry::getvk(PNK pnk, PREGTREENODE node,QString nameNK)
{
    vector<u32> list_vk;
    if(pnk->offset_list_value==0xFFFFFFFF){

        //cout<<"no value in "<<qPrintable(nameNK)<<endl;
        PVALUE pvalue=new VALUE;
        pvalue->Vname="(Default)";
        pvalue->Vtype="REG_SZ";
        pvalue->Vdata="(value not set)";
        node->value.push_back(pvalue);
    }else{
         u8 *plistvalue=gopoint(pnk->offset_list_value);
         u16 num_vk=pnk->num_value;
         PLVK plvk=(PLVK)plistvalue;
         for(int num=0;num!=num_vk;num++){
             list_vk.push_back(plvk->offset_list_vk[num]);
         }
         for(auto offset:list_vk){
             PVALUE pvalue=new VALUE;
             u8 *p=gopoint(offset);
             PVK pvk=(PVK)p;
             pvalue->Vname=getVKname(pvk);
             switch (pvk->type_data_vk){
             case REG_NONE:{pvalue->Vtype="REG_NONE";
                 string Vdata;
                if(pvk->length_data_vk<=4){
                    u16 length_data=pvk->length_data_vk;
                    for(u16 u=0;u!=length_data;u++){
                        char c=((u8*)&pvk->offsetOrdata_data_vk)[u];
                        Vdata.append(c16Tstr(c));
                        //cout<<"vdata"<<Vdata<<endl;
                        Vdata.append(" ");
                    }
                    //Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
                }else{
                    u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
                    PDATA_VK pdata_vk=(PDATA_VK)pdata;
                    u16 length_data=pvk->length_data_vk;
                    for(u16 u=0;u!=length_data;u++){
                        char c=pdata_vk->data[u];
                        Vdata.append(c16Tstr(c));
                        Vdata.append(" ");
                    }
                   // Vdata=string(pdata_vk->data,pvk->length_data_vk);
                    //cout<<"type: REG_BINARY";
                }
               pvalue->Vdata=QString::fromStdString(Vdata);
             }
                 break;
             case REG_SZ:{pvalue->Vtype="REG_SZ";
                 wstring Vdata;
                 if(pvk->length_data_vk<=4){
                     Vdata=wstring((wchar_t*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk/2);
                 }else{
                     u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
                     PDATA_VK pdata_vk=(PDATA_VK)pdata;
                     Vdata=wstring((wchar_t*)pdata_vk->data,pvk->length_data_vk/2);}
                 pvalue->Vdata=QString::fromStdWString(Vdata);
             }
                 break;
             case REG_EXPAND_SZ:{pvalue->Vtype="REG_EXPAND_SZ";
                   wstring Vdata;
                 if(pvk->length_data_vk<=4){
                     Vdata=wstring((wchar_t*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk/2);
                 }else{
                     u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
                     PDATA_VK pdata_vk=(PDATA_VK)pdata;
                     Vdata=wstring((wchar_t*)pdata_vk->data,pvk->length_data_vk/2);}
                 pvalue->Vdata=QString::fromStdWString(Vdata);
             }
                 break;
             case REG_BINARY:{pvalue->Vtype="REG_BINARY";
                  string Vdata;
                 if(pvk->length_data_vk<=4){
                     u16 length_data=pvk->length_data_vk;
                     for(u16 u=0;u!=length_data;u++){
                         char c=((u8*)&pvk->offsetOrdata_data_vk)[u];
                         Vdata.append(c16Tstr(c));
                         //cout<<"vdata"<<Vdata<<endl;
                         Vdata.append(" ");
                     }
                     //Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
                 }else{
                     u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
                     PDATA_VK pdata_vk=(PDATA_VK)pdata;
                     u16 length_data=pvk->length_data_vk;
                     for(u16 u=0;u!=length_data;u++){
                         char c=pdata_vk->data[u];
                         Vdata.append(c16Tstr(c));
                         Vdata.append(" ");
                     }
                    // Vdata=string(pdata_vk->data,pvk->length_data_vk);
                     //cout<<"type: REG_BINARY";
                 }
                pvalue->Vdata=QString::fromStdString(Vdata);
             }
                 break;
             case REG_DWORD:{pvalue->Vtype="REG_DWORD";
                  string Vdata;
                 Vdata=Tostring(pvk->offsetOrdata_data_vk);
                 pvalue->Vdata=QString::fromStdString(Vdata);
             }
                 break;
             case REG_DWORD_BIG_ENDIAN:{pvalue->Vtype="REG_DWORD_BIG_ENDIAN";
                 string Vdata;
                 if(pvk->length_data_vk<=4){
                     Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
                 }else{
                     u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
                     PDATA_VK pdata_vk=(PDATA_VK)pdata;
                     Vdata=string(pdata_vk->data,pvk->length_data_vk);}
                pvalue->Vdata=QString::fromStdString(Vdata);
             }
                 break;
             case REG_LINK:{pvalue->Vtype="REG_LINK";
                  string Vdata;
                 if(pvk->length_data_vk<=4){
                     Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
                 }else{
                     u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
                     PDATA_VK pdata_vk=(PDATA_VK)pdata;
                     Vdata=string(pdata_vk->data,pvk->length_data_vk);}
                 pvalue->Vdata=QString::fromStdString(Vdata);
             }
                 break;
             case REG_MULTI_SZ:{pvalue->Vtype="REG_MULTI_SZ";
                 wstring Vdata;
                 if(pvk->length_data_vk<=4){
                     Vdata=wstring((wchar_t*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk/2);
                 }else{
                     u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
                     PDATA_VK pdata_vk=(PDATA_VK)pdata;
                     Vdata=wstring((wchar_t*)pdata_vk->data,pvk->length_data_vk/2);}
                 pvalue->Vdata=QString::fromStdWString(Vdata);
             }
                 break;
             case REG_RESOURCE_LIST:{pvalue->Vtype="REG_RESOURCE_LIST";
                 string Vdata;
                if(pvk->length_data_vk<=4){
                    u16 length_data=pvk->length_data_vk;
                    for(u16 u=0;u!=length_data;u++){
                        char c=((u8*)&pvk->offsetOrdata_data_vk)[u];
                        Vdata.append(c16Tstr(c));
                        //cout<<"vdata"<<Vdata<<endl;
                        Vdata.append(" ");
                    }
                    //Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
                }else{
                    u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
                    PDATA_VK pdata_vk=(PDATA_VK)pdata;
                    u16 length_data=pvk->length_data_vk;
                    for(u16 u=0;u!=length_data;u++){
                        char c=pdata_vk->data[u];
                        Vdata.append(c16Tstr(c));
                        Vdata.append(" ");
                    }
                   // Vdata=string(pdata_vk->data,pvk->length_data_vk);
                    //cout<<"type: REG_BINARY";
                }
               pvalue->Vdata=QString::fromStdString(Vdata);
             }
                 break;
             case REG_FULL_RESOURCE_DESCRIPTION:{pvalue->Vtype="REG_FULL_RESOURCE_DESCRIPTION";
                 string Vdata;
                if(pvk->length_data_vk<=4){
                    u16 length_data=pvk->length_data_vk;
                    for(u16 u=0;u!=length_data;u++){
                        char c=((u8*)&pvk->offsetOrdata_data_vk)[u];
                        Vdata.append(c16Tstr(c));
                        //cout<<"vdata"<<Vdata<<endl;
                        Vdata.append(" ");
                    }
                    //Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
                }else{
                    u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
                    PDATA_VK pdata_vk=(PDATA_VK)pdata;
                    u16 length_data=pvk->length_data_vk;
                    for(u16 u=0;u!=length_data;u++){
                        char c=pdata_vk->data[u];
                        Vdata.append(c16Tstr(c));
                        Vdata.append(" ");
                    }
                   // Vdata=string(pdata_vk->data,pvk->length_data_vk);
                    //cout<<"type: REG_BINARY";
                }
               pvalue->Vdata=QString::fromStdString(Vdata);
             }
                 break;
             case REG_RESOURCE_REQUIREMENTS_LIST:{pvalue->Vtype="REG_RESOURCE_REQUIREMENTS_LIST";
                 string Vdata;
                if(pvk->length_data_vk<=4){
                    u16 length_data=pvk->length_data_vk;
                    for(u16 u=0;u!=length_data;u++){
                        char c=((u8*)&pvk->offsetOrdata_data_vk)[u];
                        Vdata.append(c16Tstr(c));
                        //cout<<"vdata"<<Vdata<<endl;
                        Vdata.append(" ");
                    }
                    //Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
                }else{
                    u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
                    PDATA_VK pdata_vk=(PDATA_VK)pdata;
                    u16 length_data=pvk->length_data_vk;
                    for(u16 u=0;u!=length_data;u++){
                        char c=pdata_vk->data[u];
                        Vdata.append(c16Tstr(c));
                        Vdata.append(" ");
                    }
                   // Vdata=string(pdata_vk->data,pvk->length_data_vk);
                    //cout<<"type: REG_BINARY";
                }
               pvalue->Vdata=QString::fromStdString(Vdata);
             }
                 break;
             case REG_QWORD:{
                  string Vdata;
                 pvalue->Vtype="REG_QWORD";
                 if(pvk->length_data_vk<=4){
                     Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
                 }else{
                     u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
                     PDATA_VK pdata_vk=(PDATA_VK)pdata;
                     u64 data=*((u64*)pdata_vk->data);
                     Vdata=Tostring(data);
                 }
                pvalue->Vdata=QString::fromStdString(Vdata);
             }
                 break;
             default:
                 break;
             }
             //cout<<"  data:"<<qPrintable(pvalue->Vdata)<<endl;

             node->value.push_back(pvalue);
         }
    }
}

string registry::c16Tstr(char c){
    char h=c;
    h=h>>4;
    h=h&0x0f;
    if(h>=0&&h<=9){
        h=h+0x30;
    }else if(h>9&&h<=15){
        h=h+'a'-10;
    }else{return "";}
    char l=c;
    l=l&0x0f;

    if(l>=0&&l<=9){
        l=l+0x30;
    }else if(l>9&&l<=15){
        l=l+'a'-10;
    }else{return "";}
    string s;
    s.append(&h,1);
   // //cout<<s<<endl;

    s.append(&l,1);
   // //cout<<" "<<s<<endl;
    return s;
}

void registry::deletedNK(u8 *p)
{
    QString dir;
    PVALUE pvalue= new VALUE;
    pvalue->Vtype="NK";
    PNK pnk=(PNK)p;
    pvalue->Vname=getNKname(pnk);
    pvalue->time=getTime((unsigned long long*)pnk->time_NK);
    if(pvalue->time==""){
        delete pvalue;
        return;
    }
    pvalue->Vtype="NK";
    pvalue->Vdata ="";
    pvalue->fatherNK=regRoot;
    if(pnk->type_nk!=0x2c&&pnk->type_nk!=0xac){
        PNK fpnk=(PNK)gopoint((pnk->offset_father));
        while(fpnk->type_nk!=0x2c&&fpnk->type_nk!=0xac){
            if(fpnk->length_name_nk>30)
                break;
            dir.insert(0,getNKname(fpnk));
            dir.insert(0,"/");
            fpnk=(PNK)gopoint(fpnk->offset_father);
        }
        dir.insert(0,"root");
        pvalue->dir=dir;
    }else{
        pvalue->dir="root";
    }
    cout<<qPrintable(pvalue->dir)<<endl;
   regRoot->value.push_back(pvalue);

}

void registry::deletedVK(u8 *p)
{
    PVALUE pvalue= new VALUE;
    PVK pvk=PVK(p);
    pvalue->Vtype="VK-";
    if(pvk->size_vk<0x18){
        delete pvalue;
        return;
    }
    pvalue->Vname=getVKname(pvk);
    if(pvalue->Vname=="error"){
        delete pvalue;
        return;
    }
    pvalue->fatherNK=regRoot;
    switch (pvk->type_data_vk){
    case REG_NONE:{pvalue->Vtype+="REG_NONE";
        string Vdata;
       if(pvk->length_data_vk<=4){
           u16 length_data=pvk->length_data_vk;
           for(u16 u=0;u!=length_data;u++){
               char c=((u8*)&pvk->offsetOrdata_data_vk)[u];
               Vdata.append(c16Tstr(c));
               //cout<<"vdata"<<Vdata<<endl;
               Vdata.append(" ");
           }
           //Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
       }else{

           u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
            if(pdata>phbin_last){
                pvalue->Vdata="<covered>";
                  break;
            }

           PDATA_VK pdata_vk=(PDATA_VK)pdata;
           u16 length_data=pvk->length_data_vk;
           if(pdata_vk->size_data<0){
              pvalue->Vdata="<covered>";
               break;
           }
           for(u16 u=0;u!=length_data;u++){
               char c=pdata_vk->data[u];
               Vdata.append(c16Tstr(c));
               Vdata.append(" ");
           }
          // Vdata=string(pdata_vk->data,pvk->length_data_vk);
           //cout<<"type: REG_BINARY";
       }
      pvalue->Vdata=QString::fromStdString(Vdata);
    }
        break;
    case REG_SZ:{pvalue->Vtype+="REG_SZ";
        wstring Vdata;
        if(pvk->length_data_vk<=4){
            Vdata=wstring((wchar_t*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk/2);
        }else{
            u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
            if(pdata>phbin_last){

                 pvalue->Vdata="<covered>";
                  break;
            }
            PDATA_VK pdata_vk=(PDATA_VK)pdata;
            if(pdata_vk->size_data<0){
               pvalue->Vdata="<covered>";
                break;
            }
            Vdata=wstring((wchar_t*)pdata_vk->data,pvk->length_data_vk/2);}
        pvalue->Vdata=QString::fromStdWString(Vdata);
    }
        break;
    case REG_EXPAND_SZ:{pvalue->Vtype+="REG_EXPAND_SZ";
          wstring Vdata;
        if(pvk->length_data_vk<=4){
            Vdata=wstring((wchar_t*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk/2);
        }else{
            u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
            if(pdata>phbin_last){
                pvalue->Vdata="<covered>";
                  break;
            }
            PDATA_VK pdata_vk=(PDATA_VK)pdata;
            if(pdata_vk->size_data<0){
               pvalue->Vdata="<covered>";
                break;
            }
            Vdata=wstring((wchar_t*)pdata_vk->data,pvk->length_data_vk/2);}
        pvalue->Vdata=QString::fromStdWString(Vdata);
    }
        break;
    case REG_BINARY:{pvalue->Vtype+="REG_BINARY";
         string Vdata;
        if(pvk->length_data_vk<=4){
            u16 length_data=pvk->length_data_vk;
            for(u16 u=0;u!=length_data;u++){
                char c=((u8*)&pvk->offsetOrdata_data_vk)[u];
                Vdata.append(c16Tstr(c));
                //cout<<"vdata"<<Vdata<<endl;
                Vdata.append(" ");
            }
            //Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
        }else{
            u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
            if(pdata>phbin_last){
                pvalue->Vdata="<covered>";
                  break;
            }
            PDATA_VK pdata_vk=(PDATA_VK)pdata;
            if(pdata_vk->size_data<0){
               pvalue->Vdata="<covered>";
                break;
            }
            u16 length_data=pvk->length_data_vk;
            for(u16 u=0;u!=length_data;u++){
                char c=pdata_vk->data[u];
                Vdata.append(c16Tstr(c));
                Vdata.append(" ");
            }
           // Vdata=string(pdata_vk->data,pvk->length_data_vk);
            //cout<<"type: REG_BINARY";
        }
       pvalue->Vdata=QString::fromStdString(Vdata);
    }
        break;
    case REG_DWORD:{pvalue->Vtype+="REG_DWORD";
         string Vdata;
        Vdata=Tostring(pvk->offsetOrdata_data_vk);
        pvalue->Vdata=QString::fromStdString(Vdata);
    }
        break;
    case REG_DWORD_BIG_ENDIAN:{pvalue->Vtype+="REG_DWORD_BIG_ENDIAN";
        string Vdata;
        if(pvk->length_data_vk<=4){
            Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
        }else{
            u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
            if(pdata>phbin_last){
                pvalue->Vdata="<covered>";
                  break;
            }
            PDATA_VK pdata_vk=(PDATA_VK)pdata;
            if(pdata_vk->size_data<0){
               pvalue->Vdata="<covered>";
                break;
            }
            Vdata=string(pdata_vk->data,pvk->length_data_vk);}
       pvalue->Vdata=QString::fromStdString(Vdata);
    }
        break;
    case REG_LINK:{pvalue->Vtype+="REG_LINK";
         string Vdata;
        if(pvk->length_data_vk<=4){
            Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
        }else{
            u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
            if(pdata>phbin_last){
                pvalue->Vdata="<covered>";
                  break;
            }
            PDATA_VK pdata_vk=(PDATA_VK)pdata;
            if(pdata_vk->size_data<0){
               pvalue->Vdata="<covered>";
                break;
            }
            Vdata=string(pdata_vk->data,pvk->length_data_vk);}
        pvalue->Vdata=QString::fromStdString(Vdata);
    }
        break;
    case REG_MULTI_SZ:{pvalue->Vtype+="REG_MULTI_SZ";
        wstring Vdata;
        if(pvk->length_data_vk<=4){
            Vdata=wstring((wchar_t*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk/2);
        }else{
            u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
            if(pdata>phbin_last){
                pvalue->Vdata="<covered>";
                  break;
            }
            PDATA_VK pdata_vk=(PDATA_VK)pdata;

            if(pdata_vk->size_data<0){
               pvalue->Vdata="<covered>";
                break;
            }
            Vdata=wstring((wchar_t*)pdata_vk->data,pvk->length_data_vk/2);}
        pvalue->Vdata=QString::fromStdWString(Vdata);
    }
        break;
    case REG_RESOURCE_LIST:{pvalue->Vtype+="REG_RESOURCE_LIST";
        string Vdata;
       if(pvk->length_data_vk<=4){
           u16 length_data=pvk->length_data_vk;
           for(u16 u=0;u!=length_data;u++){
               char c=((u8*)&pvk->offsetOrdata_data_vk)[u];
               Vdata.append(c16Tstr(c));
               //cout<<"vdata"<<Vdata<<endl;
               Vdata.append(" ");
           }
           //Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
       }else{
           u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
           if(pdata>phbin_last){
               pvalue->Vdata="<covered>";
                 break;
           }
           PDATA_VK pdata_vk=(PDATA_VK)pdata;
           if(pdata_vk->size_data<0){
              pvalue->Vdata="<covered>";
               break;
           }
           u16 length_data=pvk->length_data_vk;
           for(u16 u=0;u!=length_data;u++){
               char c=pdata_vk->data[u];
               Vdata.append(c16Tstr(c));
               Vdata.append(" ");
           }
          // Vdata=string(pdata_vk->data,pvk->length_data_vk);
           //cout<<"type: REG_BINARY";
       }
      pvalue->Vdata=QString::fromStdString(Vdata);
    }
        break;
    case REG_FULL_RESOURCE_DESCRIPTION:{pvalue->Vtype+="REG_FULL_RESOURCE_DESCRIPTION";
        string Vdata;
       if(pvk->length_data_vk<=4){
           u16 length_data=pvk->length_data_vk;

           for(u16 u=0;u!=length_data;u++){
               char c=((u8*)&pvk->offsetOrdata_data_vk)[u];
               Vdata.append(c16Tstr(c));
               //cout<<"vdata"<<Vdata<<endl;
               Vdata.append(" ");
           }
           //Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
       }else{
           u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
           if(pdata>phbin_last){
               pvalue->Vdata="<covered>";
                 break;
           }
           PDATA_VK pdata_vk=(PDATA_VK)pdata;
           if(pdata_vk->size_data<0){
              pvalue->Vdata="<covered>";
               break;
           }
           u16 length_data=pvk->length_data_vk;
           for(u16 u=0;u!=length_data;u++){
               char c=pdata_vk->data[u];
               Vdata.append(c16Tstr(c));
               Vdata.append(" ");
           }
          // Vdata=string(pdata_vk->data,pvk->length_data_vk);
           //cout<<"type: REG_BINARY";
       }
      pvalue->Vdata=QString::fromStdString(Vdata);
    }
        break;
    case REG_RESOURCE_REQUIREMENTS_LIST:{pvalue->Vtype+="REG_RESOURCE_REQUIREMENTS_LIST";
        string Vdata;
       if(pvk->length_data_vk<=4){
           u16 length_data=pvk->length_data_vk;
           for(u16 u=0;u!=length_data;u++){
               char c=((u8*)&pvk->offsetOrdata_data_vk)[u];
               Vdata.append(c16Tstr(c));
               //cout<<"vdata"<<Vdata<<endl;
               Vdata.append(" ");
           }
           //Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
       }else{
           u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
           if(pdata>phbin_last){
               pvalue->Vdata="<covered>";
                 break;
           }
           PDATA_VK pdata_vk=(PDATA_VK)pdata;
           if(pdata_vk->size_data<0){
              pvalue->Vdata="<covered>";
               break;
           }
           u16 length_data=pvk->length_data_vk;
           for(u16 u=0;u!=length_data;u++){
               char c=pdata_vk->data[u];
               Vdata.append(c16Tstr(c));
               Vdata.append(" ");
           }
          // Vdata=string(pdata_vk->data,pvk->length_data_vk);
           //cout<<"type: REG_BINARY";
       }
      pvalue->Vdata=QString::fromStdString(Vdata);
    }
        break;
    case REG_QWORD:{
         string Vdata;
        pvalue->Vtype+="REG_QWORD";
        if(pvk->length_data_vk<=4){
            Vdata=string((u8*)&pvk->offsetOrdata_data_vk,pvk->length_data_vk);
        }else{
            u8 *pdata=gopoint(pvk->offsetOrdata_data_vk);
            if(pdata>phbin_last){
                pvalue->Vdata="<covered>";
                  break;
            }
            PDATA_VK pdata_vk=(PDATA_VK)pdata;
            if(pdata_vk->size_data<0){
               pvalue->Vdata="<covered>";
                break;
            }
            u64 data=*((u64*)pdata_vk->data);
            Vdata=Tostring(data);
        }
       pvalue->Vdata=QString::fromStdString(Vdata);
    }
        break;
    default:
        pvalue->Vtype+="invalid";
        break;
    }
   regRoot->value.push_back(pvalue);
}
char *registry::regfg()
{
    if(freg.is_open()){
        PHEAD head=(PHEAD)buffer;
        return (char*)(&head->regf);
    }
    return NULL;
}


