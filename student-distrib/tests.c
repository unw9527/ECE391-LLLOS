#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "page.h"
#include "terminal.h"
#include "rtc.h"
#include "syscall.h"
#include "filesys.h"

#define PASS 1
#define FAIL 0

#define bar  printf("***************************************************************\n");

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

/* int idt_exp_test(void);
 * Inputs: void
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: test exceptions 
 * Files: x86_desc.h/S
 */
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

/* int page_content_test1(void);
 * Inputs: void
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: test page directory entry 0
 * Files: x86_desc.h/S
 */
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

/* int page_content_test2(void);
 * Inputs: void
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: test page directory entry 1
 * Files: x86_desc.h/S
 */
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

/* int page_content_test3(void);
 * Inputs: void
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: test page table entry 0
 * Files: x86_desc.h/S 1 
 */
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

/* int page_content_test4(void);
 * Inputs: void
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: test page table entry 1
 * Files: x86_desc.h/S 1 
 */
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

/* int page_dereference1(void);
 * Inputs: void
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Function: test paging when address is not null
 * Files: x86_desc.h/S 1 
 */
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

/* int page_dereference2(void);
 * Inputs: void
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Function: test paging when address is null
 * Files: x86_desc.h/S 1 
 */
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

/* void refresh_and_test(void);
 * Inputs: void
 * Return Value: none
 * Function: reset the screen and test again 
 */
void refresh_and_test()
{
	test_counter++;
	launch_tests();
}


/* int get_counter(void);
 * Inputs: void
 * Return Value: test counter
 * Function: return the test counter 
 */
int get_counter()
{
	return test_counter;
}

// add more tests here

/* Checkpoint 2 tests */
/* int rtc_cp2_test();
 * Inputs: void
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: rtc open, close, read, write
 * Files: rtc.h/c 
 */
int rtc_cp2_test(){
	int i, j;
	int fd;
	int num_char;
	int fails[4] = {0, 13, 2048}; // failure cases
	// TODO - lzy: can we ignore ENTER when executing this test?
	fd = RTC_open((uint8_t *) 0); // test RTC_open
	num_char = 8; // we just print 8 characters for 2 Hz
	for (i = 0; i <= 1024; i++){
		if (0 != RTC_write(fd, &i, 4)) continue; // test RTC_write
		else{
			clear();
			reset_cursor();
			printf("\nSetting the frequency of RTC to %u Hz\n", i);
			if (i < 256) num_char = i * 3; // triple number is suitable for display when i is small based on trials 
			else if (i < 1024) num_char = i * 2; // double number is suitable for display when i is in this range
			else num_char = i; // fit the screen limit
			for (j = 0; j < num_char; j++){
				RTC_read(fd, NULL, 0); // test RTC_read
				// if (0 != j && 0 == j % 79) printf("\n"); // 80 is the maximum number of chars in one line
				if (1 != test_counter) return PASS;
				printf("1");
			}

		}
	}
	// print some failure cases
	for (i = 0; i < 3; i++){
		clear();
		reset_cursor();
		printf("Now show some of the invalid input\n");
		for (i = 0; i < 3; i++){
			// if (1 != test_counter) return PASS;
			printf("Setting the frequency of RTC to %u Hz\n", fails[i]);
			if (0 != RTC_write(fd, &fails[i], 4)){
				printf("Invalid frequency!\n");
			}
		}
	}
	printf("\n");
	RTC_close(fd); // test RTC_close
	// clear();
	// reset_cursor();
	return PASS;

}

/* int terminal_read_test(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the function of terminal_read *
 */
int terminal_read_test(void)
{
    int8_t buf[128];
    int32_t i, size;
	int32_t fd;

    size = terminal_read(fd, buf, 0);

	printf("Buffer Echo Content\n");
    for(i = 0; i < size; i++)
        printf("%c", buf[i]);
	printf("\n\n");
    return 0;
}

/* int read_directory(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the directory of file system 
 */
int read_directory()
{
	TEST_HEADER;
	int result = PASS;
	uint8_t space = 32;
	int i;
	int32_t length;
	uint8_t buf[FILENAME_LEN+1];
	buf[FILENAME_LEN] = 0;
	int32_t fd;
	fd = open((uint8_t*)".");
	length = read(fd, buf, 0);
	while (length){
		printf("file_name:");
		for (i = 0; i < 40 - length; i++){
			putc(space);
		}
		printf("%s  ", buf);
		printf("file_type: ");
		printf("%d  ", type);
		printf("size:  ");
		printf("%d\n", file_size);
		length = read(fd, buf, 0);
	}
	close(fd);
	return result;
}

/* int test_read_file1(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the content of frame0.txt 
 */
int test_read_file1()
{
	TEST_HEADER;
	int result = PASS;
	printf("frame0.txt:\n");
	uint8_t buf[1000];
	buf[999] = 0;
	int32_t fd;
	fd = open((uint8_t*)"frame0.txt");
	read(fd, buf, 999);
	printf("%s", buf);
	close(fd);
	return result;
}

/* int test_read_file2(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the content of frame1.txt 
 */
int test_read_file2()
{
	TEST_HEADER;
	int i;
	int32_t num;
	int result = PASS;
	uint8_t buf[1000];
	int32_t fd;
	printf("frame1.txt:\n");
	for (i = 0; i < 1000; i++)
		buf[i] = 0;
	fd = open((uint8_t*)"frame1.txt");
	num = read(fd, buf, 999);
	printf("%s", buf);
	close(fd);
	return result;
}

/* int test_read_file3(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the content of verylargetextwithverylongname.txt 
 */
int test_read_file3()
{
	TEST_HEADER;
	int i;
	int32_t num;
	int result = PASS;
	uint8_t buf[2000];
	int32_t fd;
	for (i = 0; i < 2000; i++)
		buf[i] = 0;
	fd = open((uint8_t*)"verylargetextwithverylongname.txt");
	num = read(fd, buf, 1999);
	printf("%s", buf);
	return result;
}

/* int test_read_file4(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the content of verylargetextwithverylongname.txt 
 */
int test_read_file4()
{
	TEST_HEADER;
	int i;
	int32_t num;
	int32_t fd = 2;
	int result = PASS;
	uint8_t buf[2000];
	for (i = 0; i < 2000; i++)
		buf[i] = 0;
	num = read(fd, buf, 1999);
	printf("%s", buf);
	return result;
}

/* int test_read_file5(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the content of verylargetextwithverylongname.txt 
 */
int test_read_file5()
{
	TEST_HEADER;
	int i;
	int32_t num;
	int32_t fd = 2;
	int result = PASS;
	uint8_t buf[2000];
	for (i = 0; i < 2000; i++)
		buf[i] = 0;
	num = read(fd, buf, 1999);
	printf("%s", buf);
	close(fd);
	return result;
}

/* int test_read_file6(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the content of fish 
 */
int test_read_file6()
{
	TEST_HEADER;
	int i;
	int32_t num;
	int32_t fd;
	int result = PASS;
	uint8_t buf[40000];
	fd = open((uint8_t*)"fish");
	printf("fish\n");
	for (i = 0; i < 40000; i++)
		buf[i] = 0;
	num = read(fd, buf, 39999);
	for (i = 0; i < num; i++)
		putc(buf[i]);
	close(fd);
	return result;
}

/* int test_read_file7(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the content of grep 
 */
int test_read_file7()
{
	TEST_HEADER;
	int i;
	int32_t num;
	int32_t fd;
	int result = PASS;
	uint8_t buf[20];
	fd = open((uint8_t*)"grep");
	printf("grep\n");
	for (i = 0; i < 20; i++)
		buf[i] = 0;
	num = read(fd, buf, 19);
	for (i = 0; i < num; i++)
		putc(buf[i]);
	return result;
}

/* int test_read_file8(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the content of grep 
 */
int test_read_file8()
{
	TEST_HEADER;
	int i;
	int32_t num;
	int32_t fd = 2;
	int result = PASS;
	uint8_t buf[7000];
	printf("grep\n");
	for (i = 0; i < 7000; i++)
		buf[i] = 0;
	num = read(fd, buf, 6999);
	for (i = 0; i < num; i++)
		putc(buf[i]);
	close(fd);
	return result;
}

/* int test_read_file9(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the content of ls 
 */
int test_read_file9()
{
	TEST_HEADER;
	int i;
	int32_t num;
	int32_t fd;
	int result = PASS;
	uint8_t buf[20];
	fd = open((uint8_t*)"ls");
	printf("ls\n");
	for (i = 0; i < 20; i++)
		buf[i] = 0;
	num = read(fd, buf, 19);
	for (i = 0; i < num; i++)
		putc(buf[i]);
	return result;
}

/* int test_read_file10(void);
 * Inputs: void
 * Return Value: test counter
 * Function: test the content of ls 
 */
int test_read_file10()
{
	TEST_HEADER;
	int i;
	int32_t num;
	int32_t fd = 2;
	int result = PASS;
	uint8_t buf[7000];
	printf("ls\n");
	for (i = 0; i < 7000; i++)
		buf[i] = 0;
	num = read(fd, buf, 6999);
	for (i = 0; i < num; i++)
		putc(buf[i]);
	close(fd);
	return result;
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
/* void launch_test(void);
 * Inputs: void
 * Return Value: none
 * Function: launch all tests */
void launch_tests(){
	clear();
    reset_cursor();
	switch (test_counter) {			// use test_counter to determine which test
		case -2:
			bar;
			printf("                         Test Begin                            \n");
			bar;
			printf("\n\n   !!!!!  Please press ctrl + space to switch test  !!!!!! \n");
			break;
		case -1:
			bar;
			printf("                 Test 1: Terminal Test                         \n");
			bar;
			printf("\n\nFeel Free to type what you want, and press enter to see the echo of the buffer\n\n");
			while(test_counter == -1) terminal_read_test();
			break;
		case 0:
			bar;
			printf("                 Test 2: RTC Test                              \n");
			bar;
			break;
		case 1:
			rtc_cp2_test();   
			break;  
		case 2:
			bar;
			printf("                 Test 3: Read Directory Test                   \n");
			bar;
			read_directory();
			break;
		case 3:
			bar;
			printf("                 Test 4: frame0.txt Test                       \n");
			bar;
			test_read_file1();
			break;
		case 4:
			bar;
			printf("                 Test 5: frame1.txt Test                       \n");
			bar;
			test_read_file2();
			break;
		case 5:
			bar;
			printf("      Test 6: verylargetextwithverylongname.txt Test           \n");
			bar;
			printf("\nNext 3 pages are the test output for verylargetextwithverylongname.txt\n");
			break;
		case 6:
			test_read_file3();
			break;
		case 7:
			test_read_file4();
			break;
		case 8:
			test_read_file5();
			break;
		case 9:
			bar;
			printf("                    Test 7: fish Test                          \n");
			bar;
			printf("\nNext 1 page is test output for fish\n");
			break;
		case 10:
			test_read_file6();
			break;
		case 11:
			bar;
			printf("                    Test 8: grep Test                          \n");
			bar;
			printf("\nNext 2 pages are test output for grep\n");
			break;
		case 12:
			test_read_file7();
			break;
		case 13:
			test_read_file8();
			break;
		case 14:
			printf("***************************************************************\n");
			printf("                    Test 9: ls Test1                           \n");
			printf("***************************************************************\n");
			printf("\nNext 2 pages are test output for ls\n");
			break;
		case 15:
			test_read_file9();
			break;
		case 16:
			test_read_file10();
			break;
		case 17:
			bar;
			printf("                         Test End                             \n");
			bar;
		default:
			break;
	}
}



