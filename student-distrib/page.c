#include "page.h"
#include "lib.h"



/* void page_init();
 * Inputs: void
 * Return Value: none
 * Function: initialize the page */
int page_init()
{
    int i;
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
    page_table[VIDEO/BOUNDARY].kb_4_page.P = 1;
    page_table[VIDEO/BOUNDARY].kb_4_page.R_W = 1;
    /* Step3. Call the enable_page to en_pg to enable the paging.*/
    en_pg(page_directory);
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


