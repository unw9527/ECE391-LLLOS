#ifndef _PAGE_H
#define _PAGE_H
#include "types.h"

#define NUM_PAGE_ENTRY 1024
#define BOUNDARY 4096
#define SR 12
#define PDE_VAL 0x08000000 >> 22
#define USER_VIDEO_ADDR 0x08400000
#define USER_VIDEO 0x08400000 >> 22    /* The video page directory entry.*/
#define PHYSICAL_FRAME_ID   2 // 2nd frame corresponds to 8MB

typedef struct kb_4_directory_entry{
    struct{
        uint32_t P : 1;
        uint32_t R_W : 1;
        uint32_t U_S : 1;
        uint32_t PWT : 1;
        uint32_t PCD : 1;
        uint32_t A : 1;
        uint32_t Reserved : 1;
        uint32_t PS : 1;
        uint32_t G : 1;
        uint32_t Avail : 3;
        uint32_t PTBA : 20;
    } __attribute__((packed));
} kb_4_directory_entry_t;

typedef struct kb_4_page_entry{
    struct{
        uint32_t P : 1;
        uint32_t R_W : 1;
        uint32_t U_S : 1;
        uint32_t PWT : 1;
        uint32_t PCD : 1;
        uint32_t A : 1;
        uint32_t D : 1;
        uint32_t PAT : 1;
        uint32_t G : 1;
        uint32_t Avail : 3;
        uint32_t PBA : 20;
    } __attribute__((packed));
    
} kb_4_page_entry_t;

typedef struct mb_4_directory_entry{
    struct{
        uint32_t P : 1;
        uint32_t R_W : 1;
        uint32_t U_S : 1;
        uint32_t PWT : 1;
        uint32_t PCD : 1;
        uint32_t A : 1;
        uint32_t D : 1;
        uint32_t PS : 1;
        uint32_t G : 1;
        uint32_t Avail : 3;
        uint32_t PAT : 1;
        uint32_t Reserved : 9;
        uint32_t PBA : 10;
    } __attribute__((packed));
} mb_4_directory_entry_t;

typedef struct general_page_entry{
    union{
        kb_4_directory_entry_t kb_4_dir;
        kb_4_page_entry_t kb_4_page;
        mb_4_directory_entry_t mb_4_dir;
    };
} general_page_entry_t;

/* Now, create page directory and page table.*/
extern general_page_entry_t page_directory[NUM_PAGE_ENTRY] __attribute__((aligned (BOUNDARY)));
extern general_page_entry_t page_table[NUM_PAGE_ENTRY] __attribute__((aligned (BOUNDARY)));
extern general_page_entry_t video_page_table[NUM_PAGE_ENTRY] __attribute__((aligned (BOUNDARY)));

/* Here defines some extern functions used by the paging.*/

int page_init();
int32_t swap_page(uint32_t process_ct);
void flush_tlb();
int en_pg(general_page_entry_t* pt);
void set_video_page();
void store_vid_mem(int32_t term_id);
#endif

