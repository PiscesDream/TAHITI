#include <fs/fs.h>
#include <fs/hd.h>

#include <heap.h>
#include <string.h>
#include <screen.h>
#include <stddef.h>
#include <utils/system.h>
#include <string.h>
#include <stdbool.h>
#include <task.h>

file_t * fs_root, * fs_cur;
hd_info_t * main_hd;

void scan_dir(file_t *);
void init_fs() {
    fs_root = (file_t *)kmalloc(sizeof(file_t));
    
    // default: mount the hd1(master) on /
    init_hd();
    main_hd = (hd_info_t *)kmalloc(sizeof(hd_info_t));

    load_main_hd(main_hd);

    memcpy(fs_root->filename, "/        ", 8); 
    memcpy(fs_root->extension, "   ", 3); 
    fs_root->create_time = fs_root->create_date = fs_root->last_access_date = fs_root->modify_time = fs_root->modify_date = 0;
    fs_root->cluster = 0;//(main_hd->BPB_RootEntCnt * 32/main_hd->BPB_BytesPerSec) +; 
    fs_root->attr = FS_ATTR_DIRECTORY;
    fs_root->brother = fs_root->parent = fs_root->children = 0;
    fs_root->length = (main_hd->BPB_RootEntCnt * 32/main_hd->BPB_BytesPerSec) * main_hd->BPB_BytesPerSec;

    fs_cur = fs_root;
    scan_dir(fs_root);
}

void print_file(file_t *tmpf) {
    kprintf("Filename: %s, Attribute: %x, create: %x %x, last_access_date: %x, modify: %x %x, cluster: %d, length: %x\n", 
            tmpf->filename/*, tmpf->extension*/, tmpf->attr, tmpf->create_date, tmpf->create_time,
            tmpf->last_access_date, tmpf->modify_date, tmpf->modify_time, tmpf->cluster, tmpf->length
    );
}

void scan_dir(file_t * dir) {
//kprintf("scanning %s\n", dir->filename);
    void * buffer;
    uint32_t buffer_length;

    if (dir == fs_root) { 
        buffer = read_hd_continuous(2 * ((main_hd->BPB_FATSz16-1)/main_hd->BPB_SecPerClus+1) + 1 , fs_root->length);
        buffer_length = fs_root->length;
    }
    else { 
        buffer = read_hd_fat(dir->cluster, &buffer_length);
        buffer_length *= 512;
    }
    

    file_t * tmpf = (void *)kmalloc(sizeof(file_t));
    uint32_t offset;
    for (offset = 0; offset < buffer_length; offset += 32) { // 512 buffer
        memcpy((void*)tmpf, buffer + offset, 32);
        if (tmpf->attr > 0) { // file exist
//kprintf("%s[%s]\n", dir->filename, tmpf->filename);
            tmpf->brother = dir->children; dir->children = tmpf; tmpf->parent = dir;
            tmpf->children = 0;
            if (tmpf->attr & FS_ATTR_DIRECTORY) {
                if (tmpf->cluster != dir->parent->cluster && tmpf->cluster != dir->cluster)
                    scan_dir(tmpf);
                if (strncmp("..", tmpf->filename, 2)) {
                    tmpf->attr |= FS_ATTR_LINK;
                    tmpf->link = dir->parent;
                }
                else if (strncmp(".", tmpf->filename, 1)) {
                    tmpf->attr |= FS_ATTR_LINK;
                    tmpf->link = dir;
                }
            }
            tmpf = (void *)kmalloc(sizeof(file_t)); // allocate a new space
        }
    }

    kfree((uint32_t)buffer);
    kfree((uint32_t)tmpf);
}

void print_path(file_t *t) {
    if (t == fs_root)
        return;
    else {
        print_path(t->parent); 
        kprintf("/%s", t->filename);
    }
}


static void __printfilename(file_t * t, int new_line) {
    int i = 7, j;
    while (t->filename[i] == ' ') i--; 
    for (j = 0; j <= i; ++j) kprintf("%c", t->filename[j]);
    if ((t->attr & FS_ATTR_DIRECTORY) == 0) {
        kprintf("."); 
        for (i = 0; i < 3; ++i) kprintf("%c", t->extension[i]);
    }
    if (new_line) kprintf("\n");
}

void tree_for_syscall(exception_status_t * t) {
    int deep = -1, i;
    file_t * tmp = fs_cur;
    __printfilename(fs_cur, 1);
    int acc = 0;
    do {
        for (i=0;i<deep*2;++i) kprintf(" ");
        if (tmp != fs_cur) kprintf("|-");
        if ((tmp->attr & FS_ATTR_DIRECTORY) && !(tmp->attr & FS_ATTR_LINK)) {
            __printfilename(tmp,1);
            tmp = tmp->children;
            deep++;
        }
        else {
            __printfilename(tmp,1);
            if (tmp->brother) tmp = tmp->brother;
            else {tmp = tmp->parent->brother; deep--;}
        }
    } while (tmp != fs_cur && tmp);
}

static void __printfile(file_t * t) {
    int i = 7, j, acc = 0;

    kprintf("  ");  acc+=2;
    while (t->filename[i] == ' ') i--; 
    for (j = 0; j <= i; ++j) kprintf("%c", t->filename[j]); acc = i+1;
    if ((t->attr & FS_ATTR_DIRECTORY) == 0) {
        kprintf("."); 
        for (i = 0; i < 3; ++i) kprintf("%c", t->extension[i]); acc+=4;
    }
    for (; acc < 17; ++acc) kprintf(" "); 

   //attr 
    kprintf("%c%c%c%c%c%c%c%c", 
            (t->attr & FS_ATTR_LINK)?'l':'-',
            (t->attr & FS_ATTR_MOUNTPOINT)?'m':'-',
            (t->attr & FS_ATTR_ARCHIVE)?'a':'-',
            (t->attr & FS_ATTR_DIRECTORY)?'d':'-',
            (t->attr & FS_ATTR_VOLLABEL)?'v':'-',
            (t->attr & FS_ATTR_SYSTEM)?'s':'-',
            (t->attr & FS_ATTR_HIDDEN)?'h':'-',
            (t->attr & FS_ATTR_READONLY)?'r':'-'
           ); acc += 8;
    for (; acc < 32; ++acc) kprintf(" "); 

    //size
    if ((t->attr & FS_ATTR_DIRECTORY) == 0) {
        i = 0; j = t->length; while (j) {++i; j/=10;}
        if (t->length == 0) i = 1;
        kprintf("%d", t->length); acc+=i;
    }
    else {
        kprintf("dir");acc+=3;
    }
    for (; acc < 46; ++acc) kprintf(" "); 

    //date
    kprintf("%d/%d/%d %d:%d:%d",
                (t->modify_date >> 9) + 1980,
                (t->modify_date >> 5) & 0b1111,
                t->modify_date & 0b11111,
                (t->modify_time >> 10) & 0b11111,
                (t->modify_time >> 5) & 0b111111,
                (t->modify_time & 0b11111) * 2
            );
    
    //end
    kprintf("\n");
}

int ls_for_syscall(exception_status_t * t) {
    if (fs_cur == 0) 
        kprintf("The file system is not prepared!\n");
    else {
        kprintf("\ncurrent path: ");
        print_path(fs_cur);
        kprintf("/\n");

        file_t * tmp = fs_cur->children;
        //       012345678901234567890123456789012345678901234567890123456789
        kprintf("  FILE NAME        ATTRIBUTE      FILE SIZE     TIMESTAMP\n");
        kprintf("==================|==============|=============|====================\n");
        while (tmp) {
            __printfile(tmp);
            tmp = tmp->brother;
        }
        kprintf("\n");

    }return 0;
}


file_t* find_file(const char *s) {
    if (fs_cur == 0) 
        kprintf("The file system is not prepared!\n");
    else {
        file_t * tmp = fs_cur->children;
        while (tmp) {
            if (strncmp(tmp->filename, s, strlen(s)) ) 
                return tmp;
            tmp = tmp->brother;
        }
    }
    return 0;
}



int cd(const char * s) {
    if (fs_cur == 0) 
        kprintf("The file system is not prepared!\n");
    else {
        file_t * tmp = find_file(s); 
        if (tmp) {
            if (tmp->attr & FS_ATTR_LINK) 
                fs_cur = tmp->link;
            else 
                fs_cur = tmp;
            return true;
        }
        else
            kprintf("%s doesn't exist!\n", s);
    }
    return false;
}

int cat(const char * s) {
    if (fs_cur == 0) 
        kprintf("The file system is not prepared!\n");
    else {
        file_t * tmp = find_file(s); 
        if (tmp) {
//        if (tmp->attr & FS_ATTR_ARCHIVE)
            kprintf("===============FILE:%s===============\n", s);
            uint32_t cluster_count;
            char * buffer = (char *)read_hd_fat(tmp->cluster, &cluster_count);
            int i = 0;
            for (i = 0; i < tmp->length; ++i)
                scr_putch(*(buffer+i));
            kprintf("\n========END=OF=FILE:%s===============\n");

            kfree((uint32_t)buffer);
            return true;
        }
        else 
            kprintf("%s doesn't exist!\n", s);
    }
    return false;
}

int exec(const char * s) {
    if (fs_cur == 0) 
        kprintf("The file system is not prepared!\n");
    else {
        file_t * tmp = find_file(s); 
        if (tmp) {
//        if (tmp->attr & FS_ATTR_ARCHIVE)
            uint32_t cluster_count;

            char * buffer = (char *)read_hd_fat(tmp->cluster, &cluster_count);

//            print_file(tmp);
            task_t * t = create_task_from_mem((uint32_t)buffer, tmp->length+0x2000, 1); // stack size included

            // 0x3b46
            kfree((uint32_t)buffer);
            return true;
        }
        else 
            kprintf("%s doesn't exist!\n", s);
    }
    return false;
}
