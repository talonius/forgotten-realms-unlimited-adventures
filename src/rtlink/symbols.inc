; Link-time offsets in the RTLink manager segment, unless noted.
%ifndef RTLINK_SYMBOLS
%define RTLINK_SYMBOLS
manager_segment            equ 0x18de
saved_dos_es               equ 0x0b0a
saved_dos_ds               equ 0x0b0c
private_stack_segment      equ 0x0c95
private_stack_top          equ 0x0200
rtlink_init_workspace      equ 0x0820
rtlink_init_sections       equ 0x0891
rtlink_load_section        equ 0x025c
rtlink_open_file           equ 0x04e4
rtlink_section_record      equ 0x067b
rtlink_zero_paragraphs     equ 0x068b
rtlink_init_vector_check   equ 0x06c3
section_count              equ 0x0c9f
section_table              equ 0x0ca5
section_record_size        equ 0x0012
cache_slot_count           equ 0x0ca1
reload_stack_bytes         equ 0x0ca3
workspace_start            equ 0x0b14
reload_stack_used          equ 0x0af4
reload_stack_offset        equ 0x0af6
reload_stack_capacity      equ 0x0afa
cache_available            equ 0x0b08
file_open_mode             equ 0x0b09
open_file_token            equ 0x0b10
manager_flags              equ 0x0c93
overlay_start_segment      equ 0x0c77
overlay_end_segment        equ 0x0c79
start_output_offset        equ 0x0c97
start_output_segment       equ 0x0c99
end_output_offset          equ 0x0c9b
end_output_segment         equ 0x0c9d
%endif
