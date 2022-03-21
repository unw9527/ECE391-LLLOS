#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "page.h"

#define PASS 1
#define FAIL 0

#define bar  printf("\n=========================================================\n\n")

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

int test_counter = -2;

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
	asm volatile("int $0");
	return result;
}

int idt_exp_test() {
	int result = PASS;
	TEST_HEADER;
	asm volatile("int $0");
	asm volatile("int $1");
	asm volatile("int $2");
	asm volatile("int $3");
	asm volatile("int $4");
	asm volatile("int $5");
	asm volatile("int $6");
	asm volatile("int $7");
	asm volatile("int $8");
	asm volatile("int $9");
	asm volatile("int $10");
	asm volatile("int $11");
	asm volatile("int $12");
	asm volatile("int $13");
	asm volatile("int $14");
	asm volatile("int $16");
	asm volatile("int $17");
	asm volatile("int $18");
	asm volatile("int $19");	
	return result;
}

int page_content_test1(){
	TEST_HEADER;
	printf("We will show first 2 entries of directory and first entry of page table. We will also print the page corresponds to video memory.\n");
	int result = PASS;
	printf("Page directory, entry0:\n");
	printf("P:%x\nRW:%x\nU_S:%x\nPWT:%x\nPCD:%x\nA:%x\nReserved:%x\nPS:%x\nG:%x\nAvail:%x\nPTBA:%x\n", page_directory[0].kb_4_dir.P,
	page_directory[0].kb_4_dir.R_W, page_directory[0].kb_4_dir.U_S, page_directory[0].kb_4_dir.PWT, page_directory[0].kb_4_dir.PCD, 
	page_directory[0].kb_4_dir.A, page_directory[0].kb_4_dir.Reserved, page_directory[0].kb_4_dir.PS, page_directory[0].kb_4_dir.G,
	page_directory[0].kb_4_dir.Avail, page_directory[0].kb_4_dir.PTBA);
	return result;
}
int page_content_test2(){
	TEST_HEADER;
	int result = PASS;
	printf("Page directory, entry1:\n");
	printf("P:%x\nRW:%x\nU_S:%x\nPWT:%x\nPCD:%x\nA:%x\nD:%x\nPS:%x\nG:%x\nAvail:%x\nPAT:\nReserved:%x\nPBA:%x\n", page_directory[1].mb_4_dir.P,
	page_directory[1].mb_4_dir.R_W, page_directory[1].mb_4_dir.U_S, page_directory[1].mb_4_dir.PWT, page_directory[1].mb_4_dir.PCD, 
	page_directory[1].mb_4_dir.A, page_directory[1].mb_4_dir.PS, page_directory[1].mb_4_dir.PS, page_directory[1].mb_4_dir.G,
	page_directory[1].mb_4_dir.Avail, page_directory[1].mb_4_dir.PAT, page_directory[1].mb_4_dir.Reserved, page_directory[1].mb_4_dir.PBA);
	return result;
}

int page_content_test3(){
	TEST_HEADER;
	int result = PASS;
	printf("Page table, entry0:\n");
	printf("P:%x\nRW:%x\nU_S:%x\nPWT:%x\nPCD:%x\nA:%x\nD:%x\nPAT:%x\nG:%x\nAvail:%x\nPBA:%x\n", page_table[0].kb_4_page.P,
	page_table[0].kb_4_page.R_W, page_table[0].kb_4_page.U_S, page_table[0].kb_4_page.PWT, page_table[0].kb_4_page.PCD,
	page_table[0].kb_4_page.A, page_table[0].kb_4_page.D, page_table[0].kb_4_page.PAT, page_table[0].kb_4_page.G,
	page_table[0].kb_4_page.Avail, page_table[0].kb_4_page.PBA);
	return result;
}

int page_content_test4(){
	TEST_HEADER;
	int result = PASS;
	printf("Page table, video memory:\n");
	printf("P:%x\nRW:%x\nU_S:%x\nPWT:%x\nPCD:%x\nA:%x\nD:%x\nPAT:%x\nG:%x\nAvail:%x\nPBA:%x\n", page_table[VIDEO/BOUNDARY].kb_4_page.P,
	page_table[VIDEO/BOUNDARY].kb_4_page.R_W, page_table[VIDEO/BOUNDARY].kb_4_page.U_S, page_table[VIDEO/BOUNDARY].kb_4_page.PWT, 
	page_table[VIDEO/BOUNDARY].kb_4_page.PCD, page_table[VIDEO/BOUNDARY].kb_4_page.A, page_table[VIDEO/BOUNDARY].kb_4_page.D, 
	page_table[VIDEO/BOUNDARY].kb_4_page.PAT, page_table[VIDEO/BOUNDARY].kb_4_page.G, page_table[VIDEO/BOUNDARY].kb_4_page.Avail, 
	page_table[VIDEO/BOUNDARY].kb_4_page.PBA);
	return result;
}

int page_dereference1()
{
	TEST_HEADER;
	int val = 10;
	int* address;
	address = &val;
	int result = PASS;
	if (*address == val){
		printf("Address: %x\nvalue: %d\n", address, val);
	}
	else
		result = FAIL;
	return result;
}

int page_dereference2()
{
	TEST_HEADER;
	int* address;
	int val;
	address = 0;
	int result = PASS;
	val = *address;
	return result;
}

// add more tests here

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
/* Test suite entry point */
void launch_tests(){
	switch (test_counter){
		case -2:
			printf("=========================================================\n");
			printf("Press Enter to begin our test");
			break;
		case -1:
			clear();
			printf("\nPress Enter to begin our test\n");
			break;
		case 0:
			printf("\nIDT Initial Test");
			bar;
			TEST_OUTPUT("Test1: idt_test", idt_test());
			break;
		case 1:
			printf("\nIDT Exception Test");
			bar;
			TEST_OUTPUT("Test2: idt_exp_test", idt_exp_test());
			break;
		case 2:
			printf("\nPress Enter to see the RTC test\n");
			break;
		case 4:
			printf("\nPage Content Test 1\n");
			bar;
			TEST_OUTPUT("Test4: page_cotent_test", page_content_test1());
			break;
		case 5:
			printf("\nPage Content Test 2\n");
			bar;
			TEST_OUTPUT("Test4: page_cotent_test", page_content_test2());
			break;
		case 6:
			printf("\nPage Content Test 3\n");
			bar;
			TEST_OUTPUT("Test4: page_cotent_test", page_content_test3());
			break;
		case 7:
			printf("\nPage Content Test 4\n");
			bar;
			TEST_OUTPUT("Test4: page_cotent_test", page_content_test4());
			break;
		case 8:
			printf("\nPage Dereference Test: Normal\n");
			bar;
			TEST_OUTPUT("Test5: page_dereference_test_normal", page_dereference1());
			break;
		case 9:
			printf("\nPress Enter to see the Page Dereference NULL Test\n");
			break;
		case 10:
			TEST_OUTPUT("Test6: page_dereference_test_fault", page_dereference2());
			break;
		default:
			break;
	}
	// launch your tests here
}

void refresh_and_test()
{
	clear();
	reset_screen();
	test_counter++;
	launch_tests();
}

int get_counter()
{
	return test_counter;
}
