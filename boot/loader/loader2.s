    %include"../include/include.s"
    
    section loader vstart=load_address
    LOADER_STACK_TOP equ load_address
    jmp loader_start

;------------------------------------------ GDT esatablish
    GDT_BASE:   dd 0x0
                dd 0x0

    CODE_SEC:   dd 0x0000FFFF
                dd DESC_CODE_HIGH4

    DATA_STACK_DESC dd 0x0000FFFF
                    dd DESC_DATA_HIGH4

    VIDEO_DESC  dd 0x80000007
                dd DESC_VIDEO_HIGH4

    GDT_SIZE    equ $-GDT_BASE
    GDT_LIMIT   equ GDT_SIZE - 1

;----------------------------------------this for GDT spare 
    times 60 dq 0x0

;-----------------------------------------sector for the cs ss es fs
    sector_code equ (0x1 << 3) + TI_GDT + RPL0
    sector_data equ (0x2 << 3) + TI_GDT + RPL0
    sector_video    equ (0x3 << 3) +TI_GDT + RPL0

    gdt_ptr:    dw GDT_LIMIT
                dd GDT_BASE

    msg db "TW loader comes!"

;----------------------------------------print the message
loader_start:
    mov sp, load_address
    mov bp, msg
    mov cx, 15
    mov ax, 0x1301
    mov bx, 0x001f
    mov dx, 0x1800
    int 0x10

;--------------------------------------------before protection check for memory
; eax=0xe820 , edx=0x534d4150, call int 0x15 for ARDS stucture
    ARDS_BUF:   times 20 db 0
;---------------------------------------------aligned
    times   0x300-($-$$) db 0
    total_mem   dd 0

.check_mem_loop_start:
    xor ebx, ebx
    mov edx, 0x534d4150
    mov eax, 0xe820
    mov di, ARDS_BUF
    mov ecx, 20
    int 0x15

    mov edx, [total_mem]
    add edx, [ARDS_BUF+8]
    mov [total_mem], edx
    cmp ebx, 0
    jnz .check_mem_loop_start

;------------------------------------open A20
    in al, 0x92
    or al, 0b0000010
    out 0x92, al

;-------------------------------loading GDT
    lgdt [gdt_ptr]

;-----------------------------------P/E cr0 to open
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp dword sector_code : .p_mode_start

;-------------------------------protect mode set up
[ bits 32 ]
.p_mode_start:
    mov ax, sector_data
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, LOADER_STACK_TOP
    mov ax, sector_video
    mov gs, ax

    jmp $























