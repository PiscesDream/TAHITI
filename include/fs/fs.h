#ifndef _FS_H_
#define _FS_H_

#include <stdint.h>

// readonly, hidden, system, volume label, subdirectory, archive, mountpoint, link
#define FS_ATTR_READONLY   0b00000001 
#define FS_ATTR_HIDDEN     0b00000010 
#define FS_ATTR_SYSTEM     0b00000100 
#define FS_ATTR_VOLLABEL   0b00001000 
#define FS_ATTR_DIRECTORY  0b00010000 
#define FS_ATTR_ARCHIVE    0b00100000 
#define FS_ATTR_MOUNTPOINT 0b01000000 
#define FS_ATTR_LINK       0b10000000 

typedef struct __attribute__((__packed__)) __file_t{
    char filename[8];
    char extension[3];
    uint8_t attr; // readonly, hidden, system, volume label, subdirectory, archive, mountpoint, link
    uint16_t reserved;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t last_access_date;
    uint16_t ignored_in_fat12;
    uint16_t modify_time;
    uint16_t modify_date;
    uint16_t cluster;
    uint32_t length; // in bytes


    // auxiliary
    struct __file_t * parent; 
    struct __file_t * children;
    struct __file_t * brother;
    struct __file_t * link;
}file_t;

void init_fs();
int cd(const char * s);
int cat(const char * s);
int exec(const char * s);

#endif
