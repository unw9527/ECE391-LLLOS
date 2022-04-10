#ifndef _FILESYS_H
#define _FILESYS_H

#include "types.h"
#define FILENAME_LEN 32
#define NUM_DIR_ENTRIES 63
#define BLOCK_SIZE 4096
#define HALF_BLOCK_SIZE 2048
#define DESP_NUM 8
#define EXECUTE_LEN 40
#define MAGIC1 0x7F
#define MAGIC2 0x45
#define MAGIC3 0x4c
#define MAGIC4 0x46
#define HIGH_ADDR 27
#define VIRTUAL_EXECUTE_START_ADDR  0x08048000

typedef struct dentry{
    int8_t filename[FILENAME_LEN];                                   /* The file name of the directory entry.*/
    int32_t filetype;                                                /* The file type of the pointing file.*/
    int32_t inode_num;                                               /* The index of the inode.*/
    int8_t reserved[24];                                             /* The reserved bits for further use.*/
} dentry_t;


typedef struct boot_block{
    int32_t dir_count;                                               /* Number of directories.*/
    int32_t inode_count;                                             /* Number of inodes.*/
    int32_t data_count;                                              /* Number of datablocks.*/
    int8_t reserved[52];                                             /* The reserved bits for further use.*/
    dentry_t direntries[NUM_DIR_ENTRIES];                            /* All the entries are here.*/
} boot_block_t;

typedef struct inode{
    int32_t length;                                                  /* The length of bytes for the pointing file.*/
    int32_t data_block_num[BLOCK_SIZE / 4 - 1];                      /* The index of the data block.*/
} inode_t;

typedef struct data_block{
    int8_t data_array[BLOCK_SIZE];                                   /* The data array of the data block.*/
} data_block_t;

typedef struct block{                                                /* Union the three types of block into one.*/
    union{
        boot_block_t boot_block;
        inode_t inode;
        data_block_t data_block;
    };
} block_t;

typedef struct file_descriptor_entry
{
    int32_t* file_op_pt;                                             /* The specific jump table.*/
    int32_t inode;                                                   /* The inode index.*/
    int32_t file_pos;                                                /* The current position in the regular file.*/
    int32_t flags;                                                   /* The flag.*/
} file_descriptor_entry_t;

extern block_t* cast_pt;
extern int32_t type;
extern int32_t file_size;

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
int32_t open_file (const uint8_t* filename);
int32_t open_dir (const uint8_t* filename);
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
int32_t read_file(int32_t fd, void* buf, int32_t nbytes);
int32_t read_dir(int32_t fd, void* buf, int32_t nbytes);
int32_t close_file(int32_t fd);
int32_t close_dir(int32_t fd);
int32_t write_dir (int32_t fd, const void* buf, int32_t nbytes);
int32_t write_file (int32_t fd, const void* buf, int32_t nbytes);
int32_t file_loader(dentry_t* dentry);
int32_t get_length(dentry_t* dentry);
#endif
