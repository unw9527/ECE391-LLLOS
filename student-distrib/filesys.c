#include "filesys.h"
#include "lib.h"
#include "process.h"
#include "scheduling.h"
#include "terminal.h"

int32_t type;
int32_t file_size;
block_t* cast_pt;

/* int32_t read_dentry_by_name(const uint8_t*, dentry_t*);
 * Inputs: fname: The file name. dentry: The directory entry structure to be copied to.
 * Return Value: 0 if success. -1 if fail.
 * Function: Read into dentry by the file name in the directory entries. */
/* Side effect: none.*/
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry)
{
    int i;
    int32_t return_val;
    uint32_t length;
    uint32_t length_dir;
    uint32_t max_length;
    const int8_t* string;
    length = strlen((const int8_t*)fname);                           /* Get the length of the string.*/
    if (length == 0 || length > FILENAME_LEN)
        return -1;
    for (i = 0; i < NUM_DIR_ENTRIES; i++){                           /* Go through all 63 directory entries. */
        string = (const int8_t*)&(cast_pt -> boot_block.direntries[i].filename);
        length_dir = strlen((const int8_t*)string);
        max_length = (length > length_dir) ? length : length_dir;
        max_length = (max_length > FILENAME_LEN) ? FILENAME_LEN : max_length;
        if (strncmp(string, (const int8_t*)fname, max_length) == 0){ /* Compare the two strings.*/
            return_val = read_dentry_by_index(i, dentry);            /* Call the index search.*/
            if (return_val == -1)                                    /* If the sub-function cannot find, it should also return -1.*/
                return -1;
            return 0;
        }
    }
    return -1;                                                       /* The file does not exist.*/
}

/* int32_t read_dentry_by_index(const uint8_t*, dentry_t*);
 * Inputs: index: The index number of a certain directory entry. dentry: The directory entry structure to be copied to.
 * Return Value: 0 if success. -1 if fail.
 * Function: Read into dentry by the index in the directory entries. */
/* Side effect: none.*/
int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
{
    if (index > NUM_DIR_ENTRIES)                                     /* Check whether the index is invalid.*/
        return -1;
    strncpy(dentry->filename, cast_pt -> boot_block.direntries[index].filename, FILENAME_LEN); /* Copy the string to the dentry.*/
    dentry->filetype = cast_pt -> boot_block.direntries[index].filetype;        /* Copy other stuff.*/
    dentry->inode_num = cast_pt -> boot_block.direntries[index].inode_num;
    return 0;
}

/* int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
 * Inputs: inode: The index. offset: The starting point in the file.
 * Return Value: Number of bytes read into the buffer if success. -1 if fail.
 * Function: Read the data in to the buffer. */
/* Side effect: none.*/
int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
    uint32_t index;
    int32_t inode_count = cast_pt -> boot_block.inode_count;
    int32_t current_block_index;
    int32_t rest_space;
    int32_t rest_length = length;
    index = offset / BLOCK_SIZE;                                     /* Index indicates the block number in the inode.*/
    /* Check whether the inode number exceeds #inode or the index is over the corresponding limit.*/
    if (inode > inode_count - 1 || index > cast_pt[inode+1].inode.length / BLOCK_SIZE)
        return -1;
    /* Check whether the starting point is over the length of the file.*/
    if (offset > cast_pt[inode+1].inode.length)
        return 0;
    current_block_index = cast_pt[inode+1].inode.data_block_num[index];
    /* In this case, store the buffer full.*/
    if (cast_pt[inode+1].inode.length - offset >= length){
        rest_space = BLOCK_SIZE - offset % BLOCK_SIZE;               /* The rest of the space in the current block.*/
        /* In this case, copy all rest length into the buffer.*/
        if (rest_length <= rest_space){
            memcpy(buf, &(cast_pt[inode_count+current_block_index+1].data_block.data_array[offset % BLOCK_SIZE]), length);
            return length;
        }
        /* Otherwise, we need to first copy the current block into the buffer.*/
        memcpy(buf, &(cast_pt[inode_count+current_block_index+1].data_block.data_array[offset % BLOCK_SIZE]), rest_space);
        /* Update the parameters in the control.*/
        rest_length -= rest_space;
        rest_space = BLOCK_SIZE;
        index ++;
        current_block_index = cast_pt[inode+1].inode.data_block_num[index];
        /* The loop runs when rest_length is not able to be hold by the current block.*/
        while (rest_length > rest_space){
            /* In this case, we simply copy the whole block.*/
            memcpy(&buf[length-rest_length], &(cast_pt[inode_count+current_block_index+1].data_block.data_array[0]), BLOCK_SIZE);
            /* Update the parameters in the control.*/
            rest_length -= rest_space;
            rest_space = BLOCK_SIZE;
            index ++;
            current_block_index = cast_pt[inode+1].inode.data_block_num[index];
        }
        /* Finally, we copy the rest length.*/
        memcpy(&buf[length-rest_length], &(cast_pt[inode_count+current_block_index+1].data_block.data_array[0]), rest_length);
        return length;
    }
    /* In this case, store the entire file.*/
    else{
        /* The rest_length is now the rest length in the file.*/
        rest_length = cast_pt[inode+1].inode.length - offset;
        rest_space = BLOCK_SIZE - offset % BLOCK_SIZE;
        /* In this case, copy all rest length into the buffer.*/
        if (rest_length <= rest_space){
            memcpy(buf, &(cast_pt[inode_count+current_block_index+1].data_block.data_array[offset % BLOCK_SIZE]), rest_length);
            return rest_length;
        }
        /* Otherwise, we need to first copy the current block into the buffer.*/
        memcpy(buf, &(cast_pt[inode_count+current_block_index+1].data_block.data_array[offset % BLOCK_SIZE]), rest_space);
        /* Update the parameters in the control.*/
        rest_length -= rest_space;
        rest_space = BLOCK_SIZE;
        index ++;
        current_block_index = cast_pt[inode+1].inode.data_block_num[index];
        /* The loop runs when rest_length is not able to be hold by the current block.*/
        while (rest_length > rest_space){
            memcpy(&buf[cast_pt[inode+1].inode.length-offset-rest_length], &(cast_pt[inode_count+current_block_index+1].data_block.data_array[0]), BLOCK_SIZE);
            /* Update the parameters in the control.*/
            rest_length -= rest_space;
            rest_space = BLOCK_SIZE;
            index ++;
            current_block_index = cast_pt[inode+1].inode.data_block_num[index];
        }
         /* Finally, we copy the rest length.*/
        memcpy(&buf[cast_pt[inode+1].inode.length-offset-rest_length], &(cast_pt[inode_count+current_block_index+1].data_block.data_array[0]), rest_length);
        return cast_pt[inode+1].inode.length - offset;
    }
}

/* int32_t get_length(dentry_t* dentry)
 * Inputs: a pointer to a dentry struct
 * Return Value: the length of file if success; -1 otherwise
 * Side effect: none.
 */
int32_t get_length(dentry_t* dentry){
    if (NULL == dentry) {
        printf("Error: in get_length: expect a pointer to a dentry_t struct, but got a NULL pointer\n");
        return -1;
    }
    return cast_pt[dentry->inode_num+1].inode.length;
}


/* int32_t read_file(int32_t fd, void* buf, int32_t nbytes);
 * Inputs: fd: The file descriptor. buf: The buffer to store the data. nbytes: The # bytes to read.
 * Return Value: Number of bytes read into the buffer if success. -1 if fail.
 * Function: Read the data in to the buffer. */
/* Side effect: none.*/
int32_t read_file(int32_t fd, void* buf, int32_t nbytes)
{
    int32_t bytes_read;
    uint32_t inode;
    uint32_t offset;
    inode = PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].inode;
    offset = PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_pos;
    /* Call read_data to read the data into the buffer.*/
    bytes_read = read_data(inode, offset, buf, nbytes);
    if (bytes_read == -1)
        return -1;
    /* Update the file position.*/
    PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_pos += bytes_read;
    return bytes_read;
}

/* int32_t read_dir(int32_t fd, void* buf, int32_t nbytes);
 * Inputs: fd: The file descriptor. buf: The buffer to store the data. nbytes: The # bytes to read.
 * Return Value: Number of bytes read into the buffer if success. 0 indicates the end of the directory entry.
 * Function: Read the data in to the buffer. */
/* Side effect: none.*/
int32_t read_dir(int32_t fd, void* buf, int32_t nbytes)
{
    int32_t offset;
    int32_t length;
    dentry_t dentry;
    int8_t* file_name;
    offset = PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_pos;
    /* In this case, the index is out of the range of # directory entries.*/
    if (read_dentry_by_index(offset, &dentry) == -1)
        return 0;
    file_name = dentry.filename;
    length = strlen(file_name);
    if (length > 32)
        length = 32;
    /* Copy the file name into the buffer.*/
    strncpy((int8_t*)buf, file_name, FILENAME_LEN);
    /* Update the file position by 1.*/
    PCB_array[NUM_PROCESS-1-pid].thread_info.file_array[fd].file_pos ++;
    type = dentry.filetype;
    file_size = cast_pt[dentry.inode_num+1].inode.length;
    return length;
}

/* int32_t close_file(int32_t fd);
 * Inputs: fd: The file descriptor.
 * Return Value: 0.
 * Function: none. */
/* Side effect: none.*/
int32_t close_file(int32_t fd)
{
    return 0;
}

/* int32_t close_dir(int32_t fd);
 * Inputs: fd: The file descriptor.
 * Return Value: 0.
 * Function: none. */
/* Side effect: none.*/
int32_t close_dir(int32_t fd)
{
    return 0;
}

/* int32_t open_file (const uint8_t* filename);
 * Inputs: filename: The filename.
 * Return Value: 0.
 * Function: none. */
/* Side effect: none.*/
int32_t open_file (const uint8_t* filename)
{
    return 0;
}

/* int32_t open_dir (const uint8_t* filename);
 * Inputs: filename: The filename.
 * Return Value: 0.
 * Function: none. */
/* Side effect: none.*/
int32_t open_dir (const uint8_t* filename)
{
    return 0;
}

/* int32_t write_dir (int32_t fd, const void* buf, int32_t nbytes);;
 * Inputs: fd: File descriptor. buf: The buffer. nbytes: Number of bytes to write.
 * Return Value: 0.
 * Function: none. */
/* Side effect: none.*/
int32_t write_dir (int32_t fd, const void* buf, int32_t nbytes)
{
    return -1;
}

/* int32_t write_file (int32_t fd, const void* buf, int32_t nbytes);;
 * Inputs: fd: File descriptor. buf: The buffer. nbytes: Number of bytes to write.
 * Return Value: 0.
 * Function: none. */
/* Side effect: none.*/
int32_t write_file (int32_t fd, const void* buf, int32_t nbytes)
{
    return -1;
}


/*
 * iint32_t file_loader(dentry_t* dentry)
 * input: a pointer to dentry struct
 * output: 0 if success; -1 otherwise
 * effect: load the file to its virtual memory
 * side effect: none
 */
int32_t file_loader(dentry_t* dentry)
{
    int32_t file_length;
    int32_t i;
    int32_t rest_length;
    uint8_t* virtual_addr;
    uint8_t file_data[HALF_BLOCK_SIZE];
    virtual_addr = (uint8_t*) VIRTUAL_EXECUTE_START_ADDR;
    if (NULL == dentry) {
        printf("Error: in file_loader: expect a pointer to a dentry_t struct, but got a NULL pointer\n");
        return -1;
    }
    file_length = get_length(dentry);
    for (i = 0; (i+1) * HALF_BLOCK_SIZE < file_length; i++){
        read_data(dentry->inode_num, i * HALF_BLOCK_SIZE, file_data, HALF_BLOCK_SIZE);
        memcpy(&virtual_addr[HALF_BLOCK_SIZE*i], file_data, HALF_BLOCK_SIZE);
    }
    rest_length = file_length - i * HALF_BLOCK_SIZE;
    read_data(dentry->inode_num, i * HALF_BLOCK_SIZE, file_data, rest_length);
    memcpy(&virtual_addr[HALF_BLOCK_SIZE*i], file_data, rest_length);
    return 0;
}

int32_t (*read_file_pt) (int32_t, void*, int32_t) = read_file;
int32_t (*read_dir_pt) (int32_t, void*, int32_t) = read_dir;
int32_t (*open_file_pt) (const uint8_t*) = open_file;
int32_t (*open_dir_pt) (const uint8_t*) = open_dir;
int32_t (*close_file_pt) (int32_t) = close_file;
int32_t (*close_dir_pt) (int32_t) = close_dir;
int32_t (*write_file_pt) (int32_t, const void*, int32_t) = write_file;
int32_t (*write_dir_pt) (int32_t, const void*, int32_t) = write_dir;

