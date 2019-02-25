#include "keyboard.h"
#include "print.h"
#include "int.h"
#include "io.h"
#include "global.h"
#include "ioqueue.h"

// the keyboard input port is 0x60
#define KBD_BUF_PORT 0x60

// define the control character
#define esc '\033'
#define backspace '\b'
#define tab '\t'
#define enter '\r'
#define delete '\177'

// invisible character
#define ctrl_l_char 0
#define ctrl_r_char 0
#define shift_l_char 0
#define shift_r_char 0
#define alt_l_char 0
#define alt_r_char 0
#define caps_lock_char 0

// control char on and off code
#define shift_l_make 0x2a
#define shift_r_make 0x36
#define alt_l_make 0x38
#define alt_r_make 0xe038
#define alt_r_break 0xe0b8
#define ctrl_l_make 0x1d
#define ctrl_r_make 0xe01d
#define ctrl_r_break 0xe09d
#define caps_lock_make 0x3a

// init the ioqueue structure
struct ioqueue kbd_buf;

static uint8_t ctrl_status, shift_status, alt_status, caps_lock_status, ext_scancode;

static char keymap[][2]={
    {0,      0},
    {esc,   esc},
    {'1',   '!'},
    {'2',   '@'},
    {'3',   '#'},
    {'4',   '$'},
    {'5',   '%'},
    {'6',   '^'},
    {'7',   '&'},
    {'8',   '*'},
    {'9',   '('},
    {'0',   ')'},
    {'-',   '_'},
    {'=',   '+'},
    {backspace,backspace},
    {tab,   tab},
    {'q',   'Q'},
    {'w',   'W'},
    {'e',   'E'},
    {'r',   'R'},
    {'t',   'T'},
    {'y',   'Y'},
    {'u',   'U'},
    {'i',   'I'},
    {'o',   'O'},
    {'p',   'P'},
    {'[',   '{'},
    {']',   '}'},
    {enter,enter},
    {ctrl_l_char,ctrl_l_char},
    {'a',   'A'},
    {'s',   'S'},
    {'d',   'D'},
    {'f',   'F'},
    {'g',   'G'},
    {'h',   'H'},
    {'j',   'J'},
    {'k',   'K'},
    {'l',   'L'},
    {';',   ':'},
    {'\'',  '"'},
    {'`',   '~'},
    {shift_l_char,shift_l_char},
    {'\\',  '|'},
    {'z',   'Z'},
    {'x',   'X'},
    {'c',   'C'},
    {'v',   'V'},
    {'b',   'B'},
    {'n',   'N'},
    {'m',   'M'},
    {',',   '<'},
    {'.',   '>'},
    {'/',   '?'},
    {shift_r_char,shift_r_char},
    {'*',   '*'},
    {alt_l_char,alt_l_char},
    {' ',   ' '},
    {caps_lock_char,caps_lock_char},
};

static void intr_keyboard_handler(){
    
// make sure before the interrupt did the buttom pushed?
    uint8_t ctrl_down_last = ctrl_status;
    uint8_t shift_down_last = shift_status;
    uint8_t caps_lock_last = caps_lock_status;

    uint8_t break_code;
    uint16_t scancode = inb(KBD_BUF_PORT);

// if 0xe0 , many input number comes
    if(scancode == 0xe0){
        ext_scancode = 1;
        return ;
    }
    
    if(ext_scancode == 1){
        scancode = ( 0xe000 | scancode );
        ext_scancode = 0;
    }

    //put_int(scancode);

    break_code = (scancode & 0x0080);

    if(break_code){
        //put_str("breakcode\n");

        uint16_t make_code = (scancode &= 0xff7f);

        if(make_code == ctrl_l_make || make_code == ctrl_r_make){
            ctrl_status = 0;
        }
        else if(make_code == shift_l_make || make_code == shift_r_make ){
            shift_status = 0;
        }
        else if(make_code == alt_l_make || make_code == alt_r_make){
            alt_status = 0;
        }
        return ;
    }
    else if( ( scancode > 0x00 && scancode < 0x3b) || (scancode == alt_r_make) || (scancode == ctrl_r_make) ){
        uint8_t shift = 0;
        if((scancode < 0x0e) || (scancode == 0x29) || (scancode == 0x1a) || (scancode == 0x1b) || (scancode == 0x2b) || (scancode == 0x27) || (scancode == 0x28) \
            || (scancode == 0x33) || (scancode == 0x34) || (scancode == 0x35) ){
            if(shift_down_last) shift = 1;
        }
        else{
            if(shift_down_last && caps_lock_last){
                shift = 0;
            }
            else if( shift_down_last || caps_lock_last){
                shift = 1;
            }
            else{
                shift = 0;
            }
        }

        uint8_t index = (scancode & 0x00ff);

        char cur_char = keymap[index][shift];

        // for debug : put_char(cur_char);

        if(cur_char){
            if(!full_ioqueue(&kbd_buf)){
                ioq_put_char(&kbd_buf,cur_char);
            }
            return ;
        }

        if( scancode == ctrl_l_make || scancode == ctrl_r_make){
            ctrl_status = 1;
        }
        else if(scancode == shift_l_make || scancode == shift_r_make){
            shift_status = 1;
        }
        else if(scancode == alt_l_make || scancode == alt_r_make){
            alt_status = 1;
        }
        else if(scancode == caps_lock_make){
            caps_lock_status = ! caps_lock_status;
        }
    }
    else{
        put_str("unknown keys\n");
    }
}

static void intr_kbd_handler(){
    put_char('k');
    inb(KBD_BUF_PORT);
    return;
}

void keyboard_init(){
    init_ioqueue(&kbd_buf);
    register_handler( 0x21 , intr_keyboard_handler );
    put_str("keyboard init!!\n");
}










