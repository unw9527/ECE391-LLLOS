#ifndef _SYSCALL_H
#define _SYSCALL_H
#include "types.h"

/* All calls return >= 0 on success or -1 on failure. */

/*  
 * Note that the system call for halt will have to make sure that only
 * the low byte of EBX (the status argument) is returned to the calling
 * task.  Negative returns from execute indicate that the desired program
 * could not be found.
 */ 

extern int32_t jump_table_rtc[4];
extern int32_t jump_table_file[4];
extern int32_t jump_table_dir[4];
extern int32_t jump_table_terminal[4];

int32_t sys_halt (uint8_t status);
int32_t sys_execute (const uint8_t* command);
int32_t sys_read (int32_t fd, void* buf, int32_t nbytes);
int32_t sys_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t sys_open (const uint8_t* filename);
int32_t sys_close (int32_t fd);
int32_t sys_getargs (uint8_t* buf, int32_t nbytes);
int32_t sys_vidmap (uint8_t** screen_start);
int32_t sys_set_handler (int32_t signum, void* handler);
int32_t sys_sigreturn (void);
void* sys_malloc(int32_t size);
int32_t sys_free(void* addr);

#endif
