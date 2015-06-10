#include <fs/hd.h>
#include <heap.h>
#include <screen.h>

extern hd_info_t* main_hd;
static void * hd_fat_buffer;
//static int __hd_fat_buffer_sector = -1;
static uint32_t buffer_length;

void flush_fat() {
    int i = 0;
    for (i = 0; i*512 < buffer_length; ++i) 
        write_addr_to_hd(hd_fat_buffer+i*512, i+1);
}

void init_hd() {
    buffer_length = 9 * 512;//main_hd->BPB_BytesPerSec * main_hd->BPB_FATSz16;
    hd_fat_buffer = (void*)kmalloc(buffer_length);
    int i = 0;
    for (i = 0; i*512 < buffer_length; ++i) 
        read_hd_to_addr(i+1, hd_fat_buffer+i*512);
    //__hd_fat_buffer_sector = -1;
}

void load_main_hd(hd_info_t * hd) {
    read_hd_to_addr(0, hd); 
}

uint32_t get_next_cluster(int cluster) {
    uint32_t fat_offset = cluster + (cluster/2); 
    uint16_t ans = *(uint16_t*)(hd_fat_buffer+fat_offset); // danger when > 512
    if (cluster & 0x0001) 
        return ans / 16;
    else
        return ans & 0x0FFF;
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

void * read_hd_fat(uint32_t cluster, uint32_t* cluster_count) {
    uint32_t cluster_tmp = get_next_cluster(cluster);
    *cluster_count = 1;
    while (cluster_tmp < 0xff8 || cluster_tmp > 0xfff) {
        (*cluster_count)++;
        cluster_tmp = get_next_cluster(cluster_tmp);
    }
    void *addr;
    addr = (void *)kmalloc(*cluster_count * 512);

    uint32_t base_sector = 31;
    //-2 + (main_hd->BPB_RootEntCnt * 32/main_hd->BPB_BytesPerSec) + 2 * ((main_hd->BPB_FATSz16-1)/main_hd->BPB_SecPerClus+1) + 1; 
    uint32_t i = 0;
//kprintf("base sector: %d\n", base_sector); 
    for (i = 0; i < *cluster_count; ++i) {
        read_hd_to_addr(cluster+base_sector, addr+i*512); 
//kprintf("reading sector %x to %x content %x\n", cluster+base_sector,addr+i*512, *(uint32_t*)(addr+i*512));
        cluster = get_next_cluster(cluster);
    }
    return addr;
}

void write_hd_continuous(uint32_t sector, uint32_t length, void * content) { // sector, length(in byte), addr
    length = ((length-1)/512 + 1) * 512;
    uint32_t i = 0;
    for (i = 0; i < length; i += 512) 
        write_addr_to_hd(content+i, sector++); 
}

void write_hd_fat(uint32_t cluster, void * content) {
    uint32_t base_sector = 31;
    uint32_t i = 0;
    for (; cluster > 0xfff || cluster < 0xff8; ++i) {
        write_addr_to_hd(content+i*512, cluster+base_sector);
        cluster = get_next_cluster(cluster);
    }
}

void set_fat(uint32_t cluster, uint16_t data) {
    uint32_t fat_offset = cluster + (cluster/2);
    uint16_t * ans = (uint16_t*)(hd_fat_buffer+fat_offset); // danger when > 512
    if (cluster & 0x0001) 
        (*ans) = (*ans) & 0x000F | (data * 4);
    else
        (*ans) = (*ans) & 0xF000 | (data);
}

void release_fat(uint32_t cluster) {
    int next_cluster;
    do {
        next_cluster = get_next_cluster(cluster);
        set_fat(cluster, 0);
        cluster = next_cluster;
    } while (cluster > 0xfff || cluster < 0xff8);
}

int apply_fat(uint32_t cluster_count) {
    int i = 3, last_cluster = -1, first_cluster;
    while (cluster_count--) {
        while (get_next_cluster(i) != 0) i++;
        if (last_cluster == -1) 
            first_cluster = last_cluster = i;
        else {
            set_fat(last_cluster, i);
            last_cluster = i;
        }
        i++;
    }
    set_fat(last_cluster, 0xfff);
    flush_fat();
    return first_cluster;
}
