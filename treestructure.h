#ifndef TREESTRUCTURE_H
#define TREESTRUCTURE_H
#include<vector>
#include<string>
#include<QString>
enum datatype{
    REG_NONE=0,
    REG_SZ=1,
    REG_EXPAND_SZ=2,
    REG_BINARY=3,
    REG_DWORD=4,
    REG_DWORD_BIG_ENDIAN=5,
    REG_LINK=6,
    REG_MULTI_SZ=7,
    REG_RESOURCE_LIST=8,
    REG_FULL_RESOURCE_DESCRIPTION=9,
    REG_RESOURCE_REQUIREMENTS_LIST=10,
    REG_QWORD=11
};
struct regtree;
typedef struct value{
    QString Vname;
    QString Vtype;
    QString Vdata;
   // QString type;
    QString dir;
    QString time;
    regtree* fatherNK;
}VALUE,*PVALUE;

typedef struct regtree{
    QString name;
    bool isdeleted;
    std::vector<PVALUE> value;
    regtree* fatherNK;
    QString time;
    std::vector<regtree*> child;
}REGTREENODE,*PREGTREENODE;

#endif // TREESTRUCTURE_H
