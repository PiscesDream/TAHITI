#ifndef _HD_H_
#define _HD_H_

#include <stdint.h>

typedef struct __attribute__((__packed__)) __hd_info_t{
    char BS_jmpBOOT[3];
    char BS_OEMName[8];

    uint16_t BPB_BytesPerSec;
    uint8_t BPB_SecPerClus;
    uint16_t BPB_ResvdSecCnt;
    uint8_t BPB_NumFATs;
    uint16_t BPB_RootEntCnt;
    uint16_t BPB_TotSec16;
    uint8_t BPB_Media;
    uint16_t BPB_FATSz16;
    uint16_t BPB_SecPerTrk;
    uint16_t BPB_NumHeads;
    uint32_t BPB_HiddSec;
    uint32_t BPB_TotSec32;

    uint8_t BS_DrvNum;
    uint8_t BS_Reserved1;
    uint8_t BS_BootSig;
    uint32_t BS_VolID;
    char BS_VolLab[11];
    char BS_FileSysType[8];

    char bootcode[448];
    char ending[2];
} hd_info_t;

void read_hd_to_addr(uint32_t, void *);
void load_main_hd(hd_info_t *);
void init_hd();
uint32_t get_next_cluster(int cluster);
void* read_hd_continuous(uint32_t sector, uint32_t length); 
void* read_hd_fat(uint32_t cluster, uint32_t *cluster_count); 

#endif
