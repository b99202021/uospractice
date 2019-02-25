; this is the main booting MBR
; magic number for BIOS to load MBR at 0x7c

start_sector equ 0x2
load_address equ 0x900
sector_count equ 0x4

section MBR vstart=0x7c00
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov ax, 0x600
    mov bx, 0x700
    mov cx, 0
    mov dx, 0x184f

    int 0x10

; display
    mov ax, 0xb800
    mov gs, ax

    mov byte[gs:0x00], 'T'
    mov byte[gs:0x01], 0xA4
    mov byte[gs:0x02], 'W'
    mov byte[gs:0x03], 0xA4
    mov byte[gs:0x04], ' '
    mov byte[gs:0x05], 0xA4
    mov byte[gs:0x06], 'O'
    mov byte[gs:0x07], 0xA4
    mov byte[gs:0x08], 'S'
    mov byte[gs:0x09], 0xA4
    mov byte[gs:0x0a], ''
    mov byte[gs:0x0b], 0xA4
    mov byte[gs:0x0c], 'I'
    mov byte[gs:0x0d], 0xA4
    mov byte[gs:0x0e], 'S'
    mov byte[gs:0x0f], 0xA4
    mov byte[gs:0x10], ''
    mov byte[gs:0x11], 0xA4
    mov byte[gs:0x12], 'C'
    mov byte[gs:0x13], 0xA4
    mov byte[gs:0x14], 'O'
    mov byte[gs:0x15], 0xA4
    mov byte[gs:0x16], 'M'
    mov byte[gs:0x17], 0xA4
    mov byte[gs:0x18], 'I'
    mov byte[gs:0x19], 0xA4
    mov byte[gs:0x1a], 'N'
    mov byte[gs:0x1b], 0xA4
    mov byte[gs:0x1c], 'G'
    mov byte[gs:0x1d], 0xA4

    mov eax, start_sector
    mov bx, load_address
    mov cx, 4
    
    call read_disk
    jmp load_address

read_disk:
; backup cx
    mov di, cx
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
    mov ax, di
    mov dx, 256
    mul dx
    mov cx, 1024
    mov dx, 0x1f0
.go_on_read:
    in ax, dx
    mov [bx], ax
    add bx, 0x2
    loop .go_on_read
    ret

    times 510-($-$$) db 0
    db 0x55, 0xaa



















