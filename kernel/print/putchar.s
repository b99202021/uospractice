;-----------------------------------------this is for the realization of putchar

%include"/home/twchang/microos/uos/boot/include/include.s"

selector_video equ (0x3<<3)+TI_GDT+RPL0

[ bits 32] 
global put_char
put_char:
    pushad

;---------------------------------------using gs as selector of video
    mov ax, selector_video
    mov gs, ax

;-----------------------------------------get the cursor location to bx
    mov dx, 0x3d4
    mov al, 0xe
    out dx, al

    mov dx, 0x3d5
    in al, dx

    mov ah, al

    mov dx, 0x3d4
    mov al, 0xf
    out dx, al
    mov dx, 0x3d5
    in al, dx

    mov bx, ax

;----------------------------------------get the input char and handles
    mov byte al, [esp+36]

    cmp al, 0xa
    je .is_next_line

    cmp al, 0xd
    je .is_enter

    cmp al, 0x8
    je .is_backspace

    jmp .normal_case

;-------------------------------------------is_next_line handles
.is_next_line:
.is_enter:
    xor dx, dx
    mov ax, bx
    mov cx, 80

    div cx

    sub bx, dx
    
    add bx, 80

    jmp .page_roll

;----------------------------------------------is backspace
.is_backspace:
    cmp bx, 0
    je .putchar_end
    sub bx, 1
    shl bx, 1
    mov byte [gs:bx], 0x0
    inc bx
    mov byte [gs:bx], 0x7
    shr bx, 1
    jmp .page_roll

;-------------------------------------------normal case
.normal_case:
    shl bx, 1
    mov byte [gs:bx], al
    inc bx
    mov byte [gs:bx], 0x7
    inc bx
    shr bx, 1
    jmp .page_roll

;-------------------------------------------page rolling
.page_roll:
    cmp bx, 2000
;--------------------------------------------remeber that is jl instead of jle (bug)
    jl .reset_cursor
    cld
    mov ecx, 960
    mov esi, 0xc00b80a0
    mov edi, 0xc00b8000
    rep movsd

    mov ecx, 80
    mov bx, 3840
.clr:
    mov byte [gs:bx], 0x7
    inc bx
    mov byte [gs:bx], 0x20
    inc bx
    loop .clr

    mov bx, 1920

;----------------------------------------------reset cursor
.reset_cursor:
    mov dx, 0x3d4
    mov al, 0xe
    out dx, al

    mov dx, 0x3d5
    mov al, bh
    out dx, al
    
    mov dx, 0x3d4
    mov al, 0xf
    out dx, al
    
    mov dx, 0x3d5
    mov al, bl
    out dx, al

;-------------------------------------------pop registers and return controll
.putchar_end:
    popad
    ret

;-------------------------------------------the function reset the cursor according to the input
global set_cursor
set_cursor:
    mov ebx, [esp+4]
    
    mov dx, 0x3d4
    mov al, 0xe
    out dx, al

    mov dx, 0x3d5
    mov al, bh
    out dx, al

    mov dx, 0x3d4
    mov al, 0xf
    out dx, al

    mov dx, 0x3d5
    mov al, bl
    out dx, al

    ret































