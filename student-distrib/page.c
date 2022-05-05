#include "page.h"
#include "lib.h"
#include "terminal.h"
#include "scheduling.h"
#include "vbe.h"

/* void page_init();
 * Inputs: void
 * Return Value: none
 * Function: initialize the page */

general_page_entry_t page_directory[NUM_PAGE_ENTRY] __attribute__((aligned (BOUNDARY)));
general_page_entry_t page_table[NUM_PAGE_ENTRY] __attribute__((aligned (BOUNDARY)));
general_page_entry_t video_page_table[NUM_PAGE_ENTRY] __attribute__((aligned (BOUNDARY)));
general_page_entry_t rest_page_table[REST_PAGE_TABLE][NUM_PAGE_ENTRY]__attribute__((aligned (BOUNDARY)));

int page_init()
{
    int i;
    int j;
    uint32_t vbe_index = QEMU_BASE_ADDR >> 22;
    /* Step 1. Initialize the page directory.
    First set all the directory entries to be the same.*/
    for (i = 0; i < NUM_PAGE_ENTRY; i++){
        page_directory[i].kb_4_dir.P = 0;
        page_directory[i].kb_4_dir.R_W = 1;                          /* Set to read only.*/
        page_directory[i].kb_4_dir.U_S = 0;                          /* Set to supervisor privilege level.*/
        page_directory[i].kb_4_dir.PWT = 0;                          /* We always use write back cache for our design.*/
        page_directory[i].kb_4_dir.PCD = 0;                          /* Cache is always enabled in our design.*/
        page_directory[i].kb_4_dir.A = 0;                            /* According to volume3, the access bit should be initialized to 0.*/
        page_directory[i].kb_4_dir.Reserved = 0;                     /* Undefined bit.*/
        page_directory[i].kb_4_dir.PS = 0;                           /* Point to a page table.*/
        page_directory[i].kb_4_dir.G = 0;                            /* Page table do not use it.*/
        page_directory[i].kb_4_dir.Avail = 0;                        /* These bits are used by the software.*/
        page_directory[i].kb_4_dir.PTBA = i;                         /* The address of the page entry it points to, initially set to i to avoid further modification.*/
    }
    /* Then we change the entry0.*/
    page_directory[0].kb_4_dir.P = 1;
    page_directory[0].kb_4_dir.R_W = 1;
    page_directory[0].kb_4_dir.PTBA = (uint32_t)page_table >> SR;    /* This page_table address is not only the logical, linear, but also the physical address.*/
    /* Then we change the entry1.*/
    page_directory[1].mb_4_dir.P = 1;
    page_directory[1].mb_4_dir.R_W = 1;
    page_directory[1].mb_4_dir.PS = 1;
    page_directory[1].mb_4_dir.G = 1;                                /* Set since the kernel is inside this page. */
    page_directory[1].mb_4_dir.Reserved = 0;                         /* Need to set to 0.*/
    page_directory[1].mb_4_dir.PBA = 1;                              /* Set to 1 since it is the second extended page.*/
    /* Set the gui page.*/
    page_directory[vbe_index].mb_4_dir.P = 1;
    page_directory[vbe_index].mb_4_dir.R_W = 1;
    page_directory[vbe_index].mb_4_dir.PS = 1;
    page_directory[vbe_index].mb_4_dir.G = 1;                                /* Set since the kernel is inside this page. */
    page_directory[vbe_index].mb_4_dir.Reserved = 0;                         /* Need to set to 0.*/
    page_directory[vbe_index].mb_4_dir.PBA = vbe_index;                              /* Set to 1 since it is the second extended page.*/

    /* Step2. Set the page table for entry 0 of page directory.
    First we set all the page table entries to be not present.*/
    for (i = 0; i < NUM_PAGE_ENTRY; i++){
        page_table[i].kb_4_page.P = 0;                               /* From the instruction, we need to mark it as not present.*/
        page_table[i].kb_4_page.R_W = 1;
        page_table[i].kb_4_page.U_S = 0;
        page_table[i].kb_4_page.PWT = 0;
        page_table[i].kb_4_page.PCD = 0;
        page_table[i].kb_4_page.A = 0;
        page_table[i].kb_4_page.D = 0;
        page_table[i].kb_4_page.PAT = 0;
        page_table[i].kb_4_page.G = 0;
        page_table[i].kb_4_page.Avail = 0;
        page_table[i].kb_4_page.PBA = i;                             /* The address of the pages, set to i to avoid further modification.*/
    }
    /* Then we change the page entries corresponds to video memory.*/
    page_table[VIDEO/BOUNDARY + 0].kb_4_page.P = 1;
    page_table[VIDEO/BOUNDARY + 0].kb_4_page.R_W = 1;
    page_table[VIDEO/BOUNDARY + 0].kb_4_page.U_S = 1;
    page_table[VIDEO/BOUNDARY + 1].kb_4_page.P = 1;
    page_table[VIDEO/BOUNDARY + 1].kb_4_page.R_W = 1;
    page_table[VIDEO/BOUNDARY + 1].kb_4_page.U_S = 1;
    page_table[VIDEO/BOUNDARY + 2].kb_4_page.P = 1;
    page_table[VIDEO/BOUNDARY + 2].kb_4_page.R_W = 1;
    page_table[VIDEO/BOUNDARY + 2].kb_4_page.U_S = 1;
    page_table[VIDEO/BOUNDARY + 3].kb_4_page.P = 1;
    page_table[VIDEO/BOUNDARY + 3].kb_4_page.R_W = 1;
    page_table[VIDEO/BOUNDARY + 3].kb_4_page.U_S = 1;

    /* Step3. Now we set 32 MB ~ 128MB.*/
    for (i = 0; i < REST_PAGE_TABLE; i++){
        page_directory[i+BASE_OFFSET_PAGE_DIR].kb_4_dir.P = 1;
        page_directory[i+BASE_OFFSET_PAGE_DIR].kb_4_dir.U_S = 1;
        page_directory[i+BASE_OFFSET_PAGE_DIR].kb_4_dir.PTBA =(uint32_t)(&rest_page_table[i]) >> SR;
        for (j = 0; j < NUM_PAGE_ENTRY; j++){
            rest_page_table[i][j].kb_4_page.P = 1;
            rest_page_table[i][j].kb_4_page.R_W = 1;
            rest_page_table[i][j].kb_4_page.U_S = 1;
            rest_page_table[i][j].kb_4_page.PWT = 0;
            rest_page_table[i][j].kb_4_page.PCD = 0;
            rest_page_table[i][j].kb_4_page.A = 0;
            rest_page_table[i][j].kb_4_page.D = 0;
            rest_page_table[i][j].kb_4_page.PAT = 0;
            rest_page_table[i][j].kb_4_page.G = 0;
            rest_page_table[i][j].kb_4_page.Avail = 0;
            rest_page_table[i][j].kb_4_page.PBA = BASE_OFFSET_PAGE_TABLE + NUM_PAGE_ENTRY * i + j;
        }
    }

    /* Step4. Call the enable_page to en_pg to enable the paging.*/
    en_pg(page_directory);
    flush_tlb();
    return 0;
}

/*
 * int32_t swap_page(uint32_t process_ct)
 * input: the id of the current process
 * output: 0 if success; -1 otherwise
 * effect: map the virtual memory of the current process to physical memory
 * side effect: none
 */
int32_t swap_page(uint32_t process_ct){
    // enable paging at the 128MB position
    page_directory[PDE_VAL].mb_4_dir.P = 1;
    page_directory[PDE_VAL].mb_4_dir.U_S = 1;
    page_directory[PDE_VAL].mb_4_dir.R_W = 1;
    page_directory[PDE_VAL].mb_4_dir.PS = 1;
    page_directory[PDE_VAL].mb_4_dir.G = 0;  // need to flush TLB
    page_directory[PDE_VAL].mb_4_dir.Reserved = 0; // Need to set to 0
    page_directory[PDE_VAL].mb_4_dir.PBA = PHYSICAL_FRAME_ID + process_ct; 
    flush_tlb();
    return 0;
}

/*
 * int32_t set_video_page()
 * input: none
 * output: 0 if success; -1 otherwise
 * effect: map a new page onto the video memory.
 * side effect: Change the page direcotory and page.
 */
void set_video_page(int32_t term_id)
{
    /* Set the page directory for video page table.*/
    page_directory[USER_VIDEO].kb_4_dir.P = 1;
    page_directory[USER_VIDEO].kb_4_dir.R_W = 1;
    page_directory[USER_VIDEO].kb_4_dir.U_S = 1;
    page_directory[USER_VIDEO].kb_4_dir.G = 0;
    page_directory[USER_VIDEO].kb_4_dir.Reserved = 0;
    /* The virtual address is just the physical address for kernel.*/
    page_directory[USER_VIDEO].kb_4_dir.PTBA = (uint32_t)page_table >> SR;
    page_table[0].kb_4_page.P = 1;
    page_table[0].kb_4_page.U_S = 1;
    page_table[0].kb_4_page.R_W = 1;
    if (curr_terminal == term_id)
        page_table[0].kb_4_page.PBA = (uint32_t)(VIDEO / BOUNDARY);
    else
        page_table[0].kb_4_page.PBA = (uint32_t)(VIDEO / BOUNDARY + running_term + 1);
    flush_tlb();
    return;
}

void store_vid_mem(int32_t term_id){
    if(curr_terminal == term_id) {
        page_table[VIDEO / BOUNDARY].kb_4_page.PBA = (uint32_t)(VIDEO / BOUNDARY);
        page_table[VIDEO / BOUNDARY].kb_4_page.U_S = 1;
        page_table[VIDEO / BOUNDARY].kb_4_page.R_W = 1;
        page_table[VIDEO / BOUNDARY].kb_4_page.P = 1;
    }
    else {
        page_table[VIDEO / BOUNDARY].kb_4_page.PBA = (uint32_t)(VIDEO / BOUNDARY + term_id + 1);
        page_table[VIDEO / BOUNDARY].kb_4_page.U_S = 1;
        page_table[VIDEO / BOUNDARY].kb_4_page.R_W = 1;
        page_table[VIDEO / BOUNDARY].kb_4_page.P = 1;
    }
    flush_tlb();

    return;
}

/*
 * void flush_tlb()
 * input: none
 * output: none
 * effect: flush tlb (should be called every time cr3 is reloaded)
 * side effect: clobbers eax
 */
void flush_tlb() {
    asm volatile("         \n\
    movl %%cr3, %%eax      \n\
    movl %%eax, %%cr3      \n\
    "
    :
    :
    :"cc", "memory", "eax");
}


