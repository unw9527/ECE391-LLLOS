.globl jump_table_rtc, jump_table_file, jump_table_dir

jump_table_rtc:
.align 4
jump_table_file:
    .long open_file_pt
    .long read_file_pt
    .long close_file_pt

jump_table_dir:
    .long open_dir_pt
    .long read_dir_pt
    .long close_dir_pt

.end
