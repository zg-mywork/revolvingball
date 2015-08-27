#ifndef REGISTRY_H
#define REGISTRY_H
#include<fstream>
#include<iostream>
#include<sstream>
#include"registry_structure.h"
#include"treestructure.h"
#include<time.h>
#include<set>
#include<vector>
using namespace std;

class registry
{
public:
    registry();
    registry(char *fname);
    ~registry();
    bool connect(const char *fname);
    bool disconnect();
    wchar_t *filenameg();
    char *regfg();
    u32 off_root_nkg();
    u32 off_hbin_lastg();
    void analysis();
    void analysisDeleted();

    inline PREGTREENODE getRetRoot(){return regRoot;}
     //vector<PREGDELETED> getDeleted(){return vd;}
private:
    ifstream freg;
    u8 *buffer;
    u8 *phbin_first;
    u8 *pnode_tree;
    u8 *phbin_last;
    PREGTREENODE regRoot;//treenode
    //vector<PREGDELETED> vd;
private:
    u8 *gopoint(u32 offset);
    QString getHeaderName();
    QString getNKname(PNK pnk);
    QString getVKname(PVK pvk);
    int lenthVK_deleted(u8* p);
    int lenthNK_deleted(u8* p);
    void do_AD(u8* p);
    bool isleap(int year);
    QString getTime(unsigned long long*tptr);
    void traverseRegtree();// traverse tree
    void do_traverse(u8*p, PREGTREENODE node);
    void getvk(PNK pnk,PREGTREENODE node,QString nameNK);
    string c16Tstr(char c);
    template <typename T> string Tostring(T t){
        stringstream ss;
        ss<<t;
        string str;
        ss>>str;
        return str;
    }

    void deletedNK(u8 *p);
    void deletedVK(u8 *p);
};

#endif // REGISTRY_H
