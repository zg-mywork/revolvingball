/*****************************************************************************
 * Created at sept 1st,2014 by zg
 *
 * All offset paragram are relatived to the first hbin
 * **************************************************************************/

#ifndef REGISTRY_STRUCTURE_H
#define REGISTRY_STRUCTURE_H
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef  char u8;
typedef struct HEAD
{
    u32 regf;
    u32 reserved_0x04;
    u32 reserved_0x08;
    u32 time_head[2];
    //u32 time_head_end;
    u32 reserved_0x14;
    u32 reserved_0x18;
    u32 reserved_0x1c;
    u32 reserved_0x20;
    u32 offset_nk_root;
    u32 offset_hbin_last;
    u32 reserved_0x2c;
    wchar_t name_head[0x20];
}HEAD,*PHEAD;

typedef struct HEAD_HBIN
{
    u32 hbin;//"hbin"
    u32 offset_hbin_first;
    u32 size_hbin;
    u32 reserved_0x0c;
    u32 reserved_0x10;
    u32 time_head_beg;
    u32 time_head_end;
    u32 reserved_0x1c;
}HEAD_HBIN,*PHEAD_HBIN;

typedef struct NK
{
    int size_nk;
    u16 nk;//
    u16 type_nk;//------------------------------------0x06
    u32 time_NK[2];
    //u32 time_head_end;
    u32 reserved_0x10;//0x10--------------------0x10
    u32 offset_father;//----------------------0x14
    u32 num_nk_child;//--------------------------0x18
    u32 reserved_0x1c;//------------------0x1c
    u32 offset_list_child;//----------------0x20
    u32 reserved_0x24;//0x24--------------------0x24
    u32 num_value;//--------------------------0x28
    u32 offset_list_value;//----------------0x2c
    u32 offset_security;//----------------0x30
    u32 offset_name_class;//----------------0x34
    u32 reserved_0x38;
    u32 reserved_0x3c;
    u32 reserved_0x40;//
    u32 reserved_0x44;
    u32 reserved_0x48;
    u16 length_name_nk;//------------0x4c
    u16 length_class;//---------------0x4e
    u8 name_nk[1024];//-----------------------0x50
}NK,*PNK;

typedef struct lfh{
   u32 offset_list_child;
   u32 checkcodeOrname;//ignore
}LFH,*PLFH;
typedef struct rli{
    u32 offset_list_child;
}RLI,*PRLI;
typedef struct list_nk_child{
    int size;
    u16 magic;
    u16 num_child;

    union{
        LFH lfh[1024];
        RLI rli[1024];
    };
}LNKC,*PLNKC;

typedef struct list_vk{
    int size;
    u32 offset_list_vk[1024];
}LVK,*PLVK;

typedef struct vk
{
    int size_vk;//-------------------------------------0x00
    u16 vk;//-------------------------------0x04
    u16 length_name_vk;//
    u16 length_data_vk;
    u8 reserved_0x0a;//
    u8 type_vk;//
    u32 offsetOrdata_data_vk;
    u32 type_data_vk;//
    u16 is_used;//
    u16 reserved_0x16;
    u8 name_vk[1024];
}VK,*PVK;

typedef struct data_vk
{
    int size_data;
    u8 data[1024];
}DATA_VK,*PDATA_VK;

#endif // REGISTRY_H
