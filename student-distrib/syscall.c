#include "syscall.h"
#include "filesys.h"

extern int32_t jump_table_rtc[4];
extern int32_t jump_table_file[4];
extern int32_t jump_table_dir[4];

/* int32_t open(const uint8_t* filename);
 * Inputs: filename: The name of the file.
 * Return Value: -1 if not success. 0 if success.
 * Function: Open the file and fill in the information about the file for further operation. */
/* Side effect: none.*/
int32_t open (const uint8_t* filename)
{
    int32_t fd;
    int32_t (**pt)(const uint8_t*);
    dentry_t dentry;
    /* If the file does not exists, return -1.*/
    if (read_dentry_by_name(filename, &dentry) == -1)
        return -1;
    for (fd = 2; fd <= 7; fd++){
        if (file_descriptor_array[fd].flags == 0){
            break;
        }
    }
    /* If there is no free fd, return -1.*/
    if (fd == 8)
        return -1;
    switch (dentry.filetype)
    {
    /* The rtc.*/
    case 0:
        file_descriptor_array[fd].file_op_pt = (int32_t*)jump_table_rtc;
        file_descriptor_array[fd].inode = 0;
        file_descriptor_array[fd].file_pos = 0;
        file_descriptor_array[fd].flags = 1;
        break;
    /* The directory.*/
    case 1:
        file_descriptor_array[fd].file_op_pt = (int32_t*)jump_table_dir;
        file_descriptor_array[fd].inode = 0;
        file_descriptor_array[fd].file_pos = 0;
        file_descriptor_array[fd].flags = 1;
        break;
    /* The regular file.*/
    case 2:
        file_descriptor_array[fd].file_op_pt = (int32_t*)jump_table_file;
        file_descriptor_array[fd].inode = dentry.inode_num;
        file_descriptor_array[fd].file_pos = 0;
        file_descriptor_array[fd].flags = 1;
        break;
    default:
        break;
    }
    /* Call the specific open operation.*/
    pt = (int32_t (**)(const uint8_t*)) file_descriptor_array[fd].file_op_pt[0];
    (**pt)(filename);
    return fd;
}

/* int32_t close(int32_t fd);
 * Inputs: fd: The file descriptor.
 * Return Value: -1 if fd==0 or fd==1. 0 if success.
 * Function: Close the file. */
/* Side effect: Close the file.*/
int32_t close (int32_t fd)
{
    int32_t (**pt)(int32_t);
    /* If fd corresponds to stdin and stdout, return -1.*/
    if (fd == 0 || fd == 1)
        return -1;
    pt = (int32_t (**)(int32_t)) file_descriptor_array[fd].file_op_pt[2];
    /* Call the specific close function.*/
    (**pt)(fd);
    file_descriptor_array[fd].flags = 0;
    return 0;
}

/* int32_t read (int32_t fd, void* buf, int32_t nbytes);
 * Inputs: fd: The file descriptor. buf: The buffer to write. nbytes: #bytes to write.
 * Return Value: Number of bytes read. -1 indicates failure.
 * Function: Read the file. */
/* Side effect: none.*/
int32_t read (int32_t fd, void* buf, int32_t nbytes)
{
    int32_t bytes_read;
    int32_t (**pt)(int32_t, void* , int32_t);
    pt = (int32_t (**)(int32_t, void* , int32_t)) file_descriptor_array[fd].file_op_pt[1];
    /* Call the specific read function.*/
    bytes_read = (**pt) (fd, buf, nbytes);
    return bytes_read;
}

/* int32_t write (int32_t fd, const void* buf, int32_t nbytes);
 * Inputs: fd: The file descriptor. buf: The buffer to write. nbytes: #bytes to write.
 * Return Value: 0.
 * Function: Write the file. */
/* Side effect: none.*/
int32_t write(int32_t fd, const void* buf, int32_t nbytes)
{
    int32_t (**pt)(int32_t, const void*, int32_t);
    pt = (int32_t (**)(int32_t, const void*, int32_t)) file_descriptor_array[fd].file_op_pt[3];
    (**pt) (fd, buf, nbytes);
    return 0;
}
