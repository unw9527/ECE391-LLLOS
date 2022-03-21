#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "page.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}
	// asm volatile("int $0");
	return result;
}

int page_content_test(){
	TEST_HEADER;

	printf("We will show first 2 entries of directory and first entry of page table. We will also print the page corresponds to video memory.\n");
	int result = PASS;
	printf("Page directory, entry0:\n");
	printf("P:%x\nRW:%x\nU_S:%x\nPWT:%x\nPCD:%x\nA:%x\nReserved:%x\nPS:%x\nG:%x\nAvail:%x\nPTBA:%x\n", page_directory[0].kb_4_dir.P,
	page_directory[0].kb_4_dir.R_W, page_directory[0].kb_4_dir.U_S, page_directory[0].kb_4_dir.PWT, page_directory[0].kb_4_dir.PCD, 
	page_directory[0].kb_4_dir.A, page_directory[0].kb_4_dir.Reserved, page_directory[0].kb_4_dir.PS, page_directory[0].kb_4_dir.G,
	page_directory[0].kb_4_dir.Avail, page_directory[0].kb_4_dir.PTBA);
	clear();
	printf("Page directory, entry1:\n");
	printf("P:%x\nRW:%x\nU_S:%x\nPWT:%x\nPCD:%x\nA:%x\nD:%x\nPS:%x\nG:%x\nAvail:%x\nPAT:\nReserved:%x\nPBA:%x\n", page_directory[1].mb_4_dir.P,
	page_directory[1].mb_4_dir.R_W, page_directory[1].mb_4_dir.U_S, page_directory[1].mb_4_dir.PWT, page_directory[1].mb_4_dir.PCD, 
	page_directory[1].mb_4_dir.A, page_directory[1].mb_4_dir.PS, page_directory[1].mb_4_dir.PS, page_directory[1].mb_4_dir.G,
	page_directory[1].mb_4_dir.Avail, page_directory[1].mb_4_dir.PAT, page_directory[1].mb_4_dir.Reserved, page_directory[1].mb_4_dir.PBA);
	clear();
	printf("Page table, entry0:\n");
	printf("P:%x\nRW:%x\nU_S:%x\nPWT:%x\nPCD:%x\nA:%x\nD:%x\nPAT:%x\nG:%x\nAvail:%x\nPBA:%x\n", page_table[0].kb_4_page.P,
	page_table[0].kb_4_page.R_W, page_table[0].kb_4_page.U_S, page_table[0].kb_4_page.PWT, page_table[0].kb_4_page.PCD,
	page_table[0].kb_4_page.A, page_table[0].kb_4_page.D, page_table[0].kb_4_page.PAT, page_table[0].kb_4_page.G,
	page_table[0].kb_4_page.Avail, page_table[0].kb_4_page.PBA);
	clear();
	printf("Page table, video memory:\n");
	printf("P:%x\nRW:%x\nU_S:%x\nPWT:%x\nPCD:%x\nA:%x\nD:%x\nPAT:%x\nG:%x\nAvail:%x\nPBA:%x\n", page_table[VIDEO/BOUNDARY].kb_4_page.P,
	page_table[VIDEO/BOUNDARY].kb_4_page.R_W, page_table[VIDEO/BOUNDARY].kb_4_page.U_S, page_table[VIDEO/BOUNDARY].kb_4_page.PWT, 
	page_table[VIDEO/BOUNDARY].kb_4_page.PCD, page_table[VIDEO/BOUNDARY].kb_4_page.A, page_table[VIDEO/BOUNDARY].kb_4_page.D, 
	page_table[VIDEO/BOUNDARY].kb_4_page.PAT, page_table[VIDEO/BOUNDARY].kb_4_page.G, page_table[VIDEO/BOUNDARY].kb_4_page.Avail, 
	page_table[VIDEO/BOUNDARY].kb_4_page.PBA);
	return result;
}

// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	TEST_OUTPUT("idt_test", idt_test());
	asm volatile("int $0");
	/*clear();*/
	/*TEST_OUTPUT("page_cotent_test", page_content_test());*/
	// launch your tests here
}
