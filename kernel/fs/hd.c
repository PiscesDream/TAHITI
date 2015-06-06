#include <fs/hd.h>
#include <heap.h>
#include <screen.h>

extern hd_info_t* main_hd;
static void * __hd_fat_buffer = 0;
static int __hd_fat_buffer_sector = -1;

void init_hd() {
    __hd_fat_buffer = (void*)kmalloc(main_hd->BPB_BytesPerSec);
    __hd_fat_buffer_sector = -1;
}


void load_main_hd(hd_info_t * hd) {
    read_hd_to_addr(0, hd); 
}


uint32_t get_next_cluster(int cluster) {
    int cur_sector = cluster * 3/(2*main_hd->BPB_BytesPerSec)+1;
    if (cur_sector != __hd_fat_buffer_sector) {
        __hd_fat_buffer_sector = cur_sector; 
        read_hd_to_addr(cur_sector, __hd_fat_buffer);
    }

    uint16_t i1, i2, i3, answer;
    int offset = cluster/4 * 6;
    i1 = *((uint16_t*)(__hd_fat_buffer+offset));
    i2 = *((uint16_t*)(__hd_fat_buffer+offset)+1);
    i3 = *((uint16_t*)(__hd_fat_buffer+offset)+2);
    //kprintf("i1: %x, i2: %x, i3: %x ", i1, i2, i3); 
    if (cluster % 4 == 0)  
        return (i1 & 0x0fff);
    else if (cluster % 4 == 1) 
        return  ( ((i2 & 0x00ff)<<4) | ((i1 & 0xf000)>>12) );
    else if (cluster % 4 == 2) 
        return  ( ((i2 & 0xff00)>>8) | ((i3 & 0x000f)<<8) );
    else if (cluster % 4 == 3)
        return ((i3 & 0xfff0)>>4);
}

void* read_hd_continuous(uint32_t sector, uint32_t length) { // sector, length(in byte), addr
    void *addr;
    length = ((length-1)/512 + 1) * 512;
    addr = (void *)kmalloc(length);
    uint32_t i = 0;
    for (i = 0; i < length; i += 512) 
        read_hd_to_addr(sector++, addr+i); 
    return addr;
}

void* read_hd_fat(uint32_t cluster, uint32_t * cluster_count) {
    uint32_t cluster_tmp = get_next_cluster(cluster);
    *cluster_count = 1;
    while (cluster_tmp < 0xff8 || cluster_tmp > 0xfff) {
        (*cluster_count)++;
        cluster_tmp = get_next_cluster(cluster_tmp);
    }

    void *addr;
    addr = (void *)kmalloc((*cluster_count) * 512);

    uint32_t base_sector = -2 + (main_hd->BPB_RootEntCnt * 32/main_hd->BPB_BytesPerSec) + 
                2 * ((main_hd->BPB_FATSz16-1)/main_hd->BPB_SecPerClus+1) + 1; 
    uint32_t i = 0;
    for (i = 0; i < *cluster_count; ++i) {
        read_hd_to_addr(cluster+base_sector, addr+i*512); 
        cluster = get_next_cluster(cluster);
    }
    return addr;
}


