    %include"../include/include.s"
    
    section loader vstart=load_address
    LOADER_STACK_TOP equ load_address
    jmp loader_start

;------------------------------------------ GDT establish
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

;---------------------------------------------memory check
    mov ah, 0x88
    int 0x15
    and eax, 0x0000ffff
    add eax, 0x400

;------------------------------------open A20
    in al, 0x92
    or al, 0b0000010
    out 0x92, al

;-------------------------------loading GDT
    lgdt [gdt_ptr]

;-----------------------------------P/E cr0 to open (going into protect mode)
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    jmp dword sector_code : .p_mode_start

;-------------------------------protect mode set up (setup the sector section start state)
[ bits 32 ]
.p_mode_start:
    mov ax, sector_data
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov esp, LOADER_STACK_TOP
    mov ax, sector_video
    mov gs, ax

;------------------------------------------loading kernel.bin
;  eax=kernel sector , ebx=0x70000
    mov eax, KERNEL_START_SECTOR
    mov ebx, KERNEL_BIN_BASE_ADDR
    mov ecx, 200

    call read_disk

;----------------------------------------open paging (prepare PDE && PTE, page address write into cr3, set PG bit on)
    call setup_page

;--------------------------------------------add 0xc000000 to base section to map kernel into high address than 3GB
    sgdt [gdt_ptr]
    mov ebx, [gdt_ptr+2]
    or dword [ebx+0x18+4], 0xc0000000
    add dword [gdt_ptr+2], 0xc0000000
    add dword esp, 0xc0000000

;----------------------------------------------mov address to cr3 and set page on on cr0 (now the virtual address in on)
    mov eax, PAGE_DIR_TABLE_POS
    mov cr3, eax

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    lgdt [gdt_ptr]

    mov byte [gs:160], 'V'
    mov byte [gs:161], 0xA4
;----------------------------------------------reinforce to refresh the pipe
    jmp sector_code:enter_kernel

enter_kernel:
    call kernel_init
    mov esp, 0xc009f000
    jmp KERNEL_ENTRANCE

;--------------------------------------------------------move kernel to it situation considering the ELF header
kernel_init:
    xor eax, eax
    xor ebx, ebx
    xor ecx, ecx
    xor edx, edx

;------------------------------------------------------program header size
    mov dx, [KERNEL_BIN_BASE_ADDR+42]
; -------------------------------------e_phoff
    mov ebx, [KERNEL_BIN_BASE_ADDR+28]
    add ebx, KERNEL_BIN_BASE_ADDR
;------------------------------------------how many program header
    mov cx, [KERNEL_BIN_BASE_ADDR+44]

.each_segment:

;---------------------------check if the segment is NULL
    cmp byte [ebx], 0
    je .loop_end
    
    mov eax, [ebx+4]
    add eax, KERNEL_BIN_BASE_ADDR

    push dword [ebx+16]
    push eax
    push dword [ebx+8]

    call mem_cpy
    add esp, 12

.loop_end:
    add ebx, edx
    loop .each_segment
    ret

;----------------------------------------------------memory copy function
;------------------------------------------cld is for clearing the flag
mem_cpy:
    cld
    push ebp
    mov ebp, esp
    push ecx

    mov edi, [ebp+8]
    mov esi, [ebp+12]
    mov ecx, [ebp+16]
    rep movsb

    pop ecx
    pop ebp
    ret

;-------------------------------------------------------clear space for PDE
setup_page:
    mov ecx, 4096
    mov esi, 0
.clear_page_dir:
    mov byte [PAGE_DIR_TABLE_POS+esi], 0
    inc esi
    loop .clear_page_dir

;-------------------------------------------------------get pageing directory entry for double paging
.create_pde:
    mov edx, PG_P | PG_WR_W | PG_US_U
    mov eax, PAGE_DIR_TABLE_POS
    add eax, 0x1000
    or eax, edx

;--------------------------------------------------------map the 0 and 768 sector to the low address
    mov dword [PAGE_DIR_TABLE_POS], eax
    mov dword [PAGE_DIR_TABLE_POS+0xc00], eax
    sub eax, 0x1000

;--------------------------------------------------------let the last page entry point to itself for modification
    mov [PAGE_DIR_TABLE_POS + 4092], eax

;---------------------------------------------------------setup PTE page table entry 
    mov ecx, 256
    mov esi, 0
    mov edx, PG_P | PG_WR_W | PG_US_U
    mov ebx, PAGE_DIR_TABLE_POS + 0x1000
.create_pte:
    mov [ebx+4*esi], edx
    inc esi
    add edx, 4096
    loop .create_pte

;----------------------------------------------------------map the rest kernel space to real space (total 254*4MB)
    mov ecx, 254
    mov esi, 0xc01
    mov eax, PAGE_DIR_TABLE_POS+0x2000
    or eax, PG_P | PG_WR_W | PG_US_U 
.create_kernel_pde:
    mov [PAGE_DIR_TABLE_POS+4*esi], eax
    add eax, 0x1000
    loop .create_kernel_pde
    ret

;-------------------------------------------------------------read_disk function m32
;   port 0x1f2 for sector count
;   port 0x1f3 for LBA low
;   port 0x1f3 for LBA mid
;   port 0x1f4 for LBA high
;   port 0x1f6 for 4 bit eax and the rest are 1 MOD 1 DEV
;   port 0x1f7 is command and should be written the last one 

read_disk:
; backup cx
    mov edi, ecx
; write the command to the IO port
    mov dx, 0x1f3
    out dx, al

    shr eax, 0x8
    mov dx, 0x1f4
    out dx, al

    shr eax, 0x8
    mov dx, 0x1f5
    out dx, al

    shr eax, 0x8
    or al, 0xe0
    mov dx, 0x1f6
    out dx, al

    mov al, cl
    mov dx, 0x1f2
    out dx, al

    mov al, 0x20
    mov dx, 0x1f7
    out dx, al

;read status from hard disk
.not_ready:
    in al, dx
    and al, 0x88
    cmp al, 0x08
    jnz .not_ready

;read from disk
    mov eax, edi
    mov edx, 256
    mul edx
    mov ecx, eax
    mov dx, 0x1f0
.go_on_read:
    in ax, dx
    mov [ebx], ax
    add ebx, 0x2
    loop .go_on_read
    ret




















