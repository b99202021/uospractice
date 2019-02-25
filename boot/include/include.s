;-----------------------------------loader
start_sector equ 0x2
load_address equ 0x900
sector_count equ 0x4
;-----------------------------------gdt ocnstant
DESC_G_4K equ 0b100000000000000000000000
DESC_D_32 equ 0b10000000000000000000000
DESC_L equ 0x0
DESC_AVL equ 0x0
DESC_LIMIT_CODE2 equ 0b11110000000000000000
DESC_LIMIT_DATA2 equ 0b11110000000000000000
DESC_LIMIT_VIDEO2 equ 0b1011
DESC_P equ 0b1000000000000000
DESC_DPL_0 equ 0b0
DESC_DPL_1 equ 0b10000000000000
DESC_DPL_2 equ 0b100000000000000
DESC_DPL_3 equ 0b110000000000000
DESC_S_CODE equ 0b1000000000000
DESC_S_DATA equ 0b1000000000000
DESC_S_SYS equ 0b0
DESC_TYPE_CODE equ 0b100000000000
DESC_TYPE_DATA equ 0b1000000000

DESC_CODE_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_CODE2 + DESC_P + DESC_DPL_0 + DESC_S_CODE + DESC_TYPE_CODE + 0x00
DESC_DATA_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_DATA2 + DESC_P + DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x00
DESC_VIDEO_HIGH4 equ (0x00 << 24) + DESC_G_4K + DESC_D_32 + DESC_L + DESC_AVL + DESC_LIMIT_VIDEO2 + DESC_P + DESC_DPL_0 + DESC_S_DATA + DESC_TYPE_DATA + 0x00

;`------------------------------------selector
RPL0 equ 0b0
RPL1 equ 0b1
RPL2 equ 0b10 
RPL3 equ 0b11

TI_GDT equ 0b0
TI_IGT equ 0b100

;---------------------------------------loader and kernel
PAGE_DIR_TABLE_POS equ 0x100000

;----------------------------------------paging property PG_P 表示已經放入記憶體中
PG_P equ 0b1
PG_WR_R equ 0b0
PG_WR_W equ 0b10
PG_US_S equ 0b0
PG_US_U equ 0b100

;---------------------------------------------kernel address
KERNEL_START_SECTOR equ 0x9
KERNEL_BIN_BASE_ADDR equ 0x70000
KERNEL_ENTRANCE equ 0xc0001500
