[ bits 32 ]

%define ERROR_CODE nop

;--------------------------------------------- this is the error code information
%define ZERO push 0

;---------------------------------------------include the put_str function

extern put_str

;---------------------------------------------include the idt_handling table using c language to implent the error handling code
extern idt_table

section .data
;------------------------------------------------now we have to define the macro for the interrupt code
;------------------------------------------------as we know 

%macro VECTOR 2

section .text
intr%1entry:
    %2
    push ds
    push es
    push fs
    push gs
    pushad

;----------------------------------------send the end of interrupt message to main and slave
;----------------------------------------notice that the AEOI bit in ICW4 should be 0
    mov al, 0x20
    out 0xa0, al
    out 0x20, al

    push dword %1
    call [idt_table+4*%1]

    jmp intr_exit

;-----------------------------------------iret is a special instruction for return from interrupt

;-----------------------------------------record the entry address for the interrupt description table
section .data
    dd intr%1entry

%endmacro

;-----------------------------------------since calling c function would mess up the stack and register we have to load and restore it
;-----------------------------------------intr_exit is for restoring

;-----------------------------------------make intr_exit global
global intr_exit

intr_exit:
    add esp, 4
    popad
    pop gs
    pop fs
    pop es
    pop ds
    add esp, 0x4
;-----------------------------------------iretd return the intstruction pointer, code segment, esp ,ss and eflags (program status)
    iretd

global intr_entry_table

;-----------------------------------------the interrupt handlers
intr_entry_table:

VECTOR 0x0,ZERO
VECTOR 1,ZERO
VECTOR 2,ZERO
VECTOR 3,ZERO
VECTOR 4,ZERO
VECTOR 5,ZERO
VECTOR 6,ZERO
VECTOR 7,ZERO
VECTOR 8,ZERO
VECTOR 9,ZERO
VECTOR 10,ZERO
VECTOR 11,ZERO
VECTOR 12,ZERO
VECTOR 13,ZERO
VECTOR 14,ZERO
VECTOR 15,ZERO
VECTOR 16,ZERO
VECTOR 17,ZERO
VECTOR 18,ZERO
VECTOR 19,ZERO
VECTOR 20,ZERO
VECTOR 21,ZERO
VECTOR 22,ZERO
VECTOR 23,ZERO
VECTOR 24,ZERO
VECTOR 25,ZERO
VECTOR 26,ZERO
VECTOR 27,ZERO
VECTOR 28,ZERO
VECTOR 29,ZERO
VECTOR 30,ZERO
VECTOR 31,ZERO
VECTOR 32,ZERO   ;----------------------the time interrupt entry

VECTOR 33,ZERO
VECTOR 34,ZERO
VECTOR 35,ZERO
VECTOR 36,ZERO
VECTOR 37,ZERO
VECTOR 38,ZERO
VECTOR 39,ZERO
VECTOR 40,ZERO
VECTOR 41,ZERO
VECTOR 42,ZERO
VECTOR 43,ZERO
VECTOR 44,ZERO
VECTOR 45,ZERO
VECTOR 46,ZERO
VECTOR 47,ZERO
VECTOR 48,ZERO

;---------------------------------------this is for 0x80 interrupt 

[bits 32]

extern syscall_table

section .text

global syscall_handler

syscall_handler:

;go into the syscall
    push 0
    
    push ds
    push es
    push fs
    push gs

;---------------------------------------push all the registers into stack
    pushad

    push 0x80

;---------------------------------------the parameters for syscalls
    push eax
    push ecx
    push ebx

;---------------------------------------call syscall handler
    call [syscall_table + eax * 4]

;---------------------------------------please remeber to add the stack back to place
    add esp, 12
    
;---------------------------------------set the return value for syscall to eax
    mov [esp + 8*4], eax
    
    jmp intr_exit







