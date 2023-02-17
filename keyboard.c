#include <spede/stdio.h>
#include <spede/machine/io.h>

#include "kernel.h"
#include "keyboard.h"

// Keyboard data port
#define KBD_PORT_DATA           0x60

// Keyboard status port
#define KBD_PORT_STAT           0x64

// Keyboard scancode definitions
#define KEY_CTRL_L              0x1D
#define KEY_CTRL_R              0xE01D

#define KEY_ALT_L               0x38
#define KEY_ALT_R               0xE038

#define KEY_SHIFT               0x04
#define KEY_SHIFT_L             0x2A
#define KEY_SHIFT_R             0x36

#define KEY_CAPS                0x3A
#define KEY_NUMLOCK             0x45
//NEW definitions
#define KEY_RELEASED(c) ((c & 0x80)!=0) // compare the 7-bit of C with 0x80 (1000 0000)
                                        // Next information provided by the 1.1 Key release (link provided):
                                        // The scancode for key release (`break') is obtained from
                                        // it by setting the high order bit (adding 0x80 = 128)
#define KEY_PRESSED(c) ((c & 0x80) ==0) //
#define KEY_BACKSPACE 0x08
#define KEY_TAB 0x09
#define KEY_CR 0x0D // Carriage Return
#define KEY_NL_LF 0x0A // New line/line Feed
//Define tracking variables for CTRL,ALT,SHIFT,CAPS and gives a bit
#define TRACK_CTRL 0x01  // 0001
#define TRACK_ALT 0X02 // 0010
#define TRACK_SHIFT 0x04 // 0100
#define TRACK_CAPS 0x08// 0000 1000
#define TRACK_NUMLOCK 0x10 // 0001 0000
// hello
//Define tracking variable for Tracking CTRL,ALT,SHIFT,CAPS,NUMLOCK
unsigned int tracking_variable;

//We have different buffers to track the different keys
//1) Track when we do not have SHIFT,CAP,NUMLOCK
char key_map_NSC[]={
    KEY_NULL,KEY_ESCAPE, '1','2','3','4','5','6','7','8','9','0','-','=',KEY_BACKSPACE, KEY_TAB,
    'q','w','e','r','t','y','u','i','o','p','[',']',KEY_NL_LF, KEY_NULL,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', KEY_NULL,'\\' ,
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_NULL,KEY_NULL,KEY_NULL,' ',KEY_NULL,
    KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_NULL, KEY_NULL,
    '7',KEY_UP,'9','-',KEY_LEFT,'5',KEY_RIGHT,'+','1',KEY_DOWN,'3',KEY_INSERT,KEY_DELETE,
    KEY_NULL,KEY_NULL,KEY_NULL, KEY_F11,KEY_F12
};

//2) When we set shift
char key_map_shift[] = {
    KEY_NULL,KEY_ESCAPE, '!','@','#','$','%','^','&','*','(',')','_','+',KEY_BACKSPACE, KEY_TAB,
    'Q','W','E','R','T','Y','U','I','O','P','{','}',KEY_NL_LF, KEY_TAB,
    'A', 'S', 'D', 'D', 'G', 'H', 'J', 'K', 'L', ':','"','~',KEY_NULL,'|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', KEY_NULL,KEY_NULL,KEY_NULL,' ',KEY_NULL,
    KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_NULL, KEY_NULL,
    '7',KEY_UP,'9','-',KEY_LEFT,'5',KEY_RIGHT,'+','1',KEY_DOWN,'3',KEY_INSERT,KEY_DELETE,
    KEY_NULL,KEY_NULL,KEY_NULL, KEY_F11,KEY_F12
};


// When we set Caps lock
char key_map_cap[]={
    KEY_NULL,KEY_ESCAPE, '1','2','3','4','5','6','7','8','9','0','-','=',KEY_BACKSPACE, KEY_TAB,
    'Q','W','E','R','T','Y','U','I','O','P','{','}',KEY_NL_LF, KEY_TAB,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', KEY_NULL,'\\',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', KEY_NULL,KEY_NULL,KEY_NULL,' ',KEY_NULL,
    KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_NULL, KEY_NULL,
    '7',KEY_UP,'9','-',KEY_LEFT,'5',KEY_RIGHT,'+','1',KEY_DOWN,'3',KEY_INSERT,KEY_DELETE,
    KEY_NULL,KEY_NULL,KEY_NULL, KEY_F11,KEY_F12
};

// When we set shift and Cap
char key_map_shiftCap []= {
    KEY_NULL,KEY_ESCAPE, '!','@','#','$','%','^','&','*','(',')','_','+',KEY_BACKSPACE, KEY_TAB,
    'q','w','e','r','t','y','u','i','o','p','[',']',KEY_NL_LF, KEY_TAB,
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':','"','~',KEY_NULL,'|',
	'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_NULL,KEY_NULL,KEY_NULL,' ',KEY_NULL,
    KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_NULL, KEY_NULL,
	'7',KEY_UP,'9','-',KEY_LEFT,'5',KEY_RIGHT,'+','1',KEY_DOWN,'3',KEY_INSERT,KEY_DELETE,
	KEY_NULL,KEY_NULL,KEY_NULL, KEY_F11,KEY_F12
};

// when we set ALT
char key_map_alt []={

};

// map when we have NUM lock
//
char key_map_numLock []= {
    KEY_NULL,KEY_ESCAPE, '1','2','3','4','5','6','7','8','9','0','-','=',KEY_BACKSPACE, KEY_TAB,
    'q','w','e','r','t','y','u','i','o','p','[',']',KEY_NL_LF, KEY_NULL,
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', KEY_NULL,'\\' ,
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', KEY_NULL,KEY_NULL,KEY_NULL,' ',KEY_NULL,
    KEY_F1,KEY_F2,KEY_F3,KEY_F4,KEY_F5,KEY_F6,KEY_F7,KEY_F8,KEY_F9,KEY_F10,KEY_NULL, KEY_NULL,
    '7',KEY_UP,'9','-',KEY_LEFT,'5',KEY_RIGHT,'+','1',KEY_DOWN,'3',KEY_INSERT,KEY_DELETE,
    KEY_NULL,KEY_NULL,KEY_NULL, KEY_F11,KEY_F12
};

/*
 * Initializes keyboard data structures and variables
 */
void keyboard_init() {
    kernel_log_info("Initializing keyboard");
    tracking_variable= 0x00; // Init the Track variable
}

/**
 * Scans for keyboard input and returns the raw character data
 * @return raw character data from the keyboard
 */
unsigned int keyboard_scan(void) {
    //unsigned int c = KEY_NULL;
    // We need to scan for keyboard
    // keeping track of which key is pressed
    //NOTE: We do not get the ascci value

    unsigned int c= inportb(KBD_PORT_DATA); // We scan keyboard to store key map value into c
                                            // We say that we really got the value by setting 7'b
                                            //which means
                                            //realeased key = we entered the information
                                            // (but can look it as pressed to make things easier)
    return c;
}

/**
 * Polls for a keyboard character to be entered.
 *
 * If a keyboard character data is present, will scan and return
 * the decoded keyboard output.
 *
 * @return decoded character or KEY_NULL (0) for any character
 *         that cannot be decoded
 */
unsigned int keyboard_poll(void) {

    unsigned int c = KEY_NULL;
    char status= inportb(KBD_PORT_STAT); // Read status

    // Status we will simply be concerned with bit 0
    // which if set indicates that keyboard data is available
    // if the bit is set
    if((status & 0x01) != 0){     //if status bit 0 is set (data avaible)
                                 // if status bit 0 is not set (data not avaible)
        c=keyboard_scan();       // call scan function
        c=keyboard_decode(c);    // call decode function        return c;
        return c;
    }
    return KEY_NULL;
}

/**
 * Blocks until a keyboard character has been entered
 * @return decoded character entered by the keyboard or KEY_NULL
 *         for any character that cannot be decoded
 */
unsigned int keyboard_getc(void) {
    unsigned int c = KEY_NULL;

    while ((c = keyboard_poll()) == KEY_NULL);
    c= keyboard_decode(c);
    return c;
}

/**
 * Processes raw keyboard input and decodes it.
 *
 * Should keep track of the keyboard status for the following keys:
 *   SHIFT, CTRL, ALT, CAPS, NUMLOCK
 *
 * For all other characters, they should be decoded/mapped to ASCII
 * or ASCII-friendly characters.
 *
 * For any character that cannot be mapped, KEY_NULL should be returned.
 */
unsigned int keyboard_decode(unsigned int c) {
    // c has the keyboard value (0x01,0x02, 0x03 etc.)
    // Note: C has the 7'b set (we need to clear 7'b to get the real key)

    unsigned int key_pressed= KEY_PRESSED(c); //Track 7'b if pressed
    unsigned int key_released= KEY_RELEASED(c);// track 7'b if released
    c= (c & ~0x80); // clear 7'b bit to have real key value
                    // 0x80=          0111 1111
                    // c=             1110 1000    (AND)
                    // -------------------------
                    // c & ~0x80      0010 1000
    //this switch case is if for change specialKey_status
    //tracking_variable beggins with 0x00; (first time)
    //c=          1100 1000 
    //KEY_CTRL_L= 0100 0000
    switch (c) {
        case KEY_CTRL_R: // Need to add other a ctrl
                //Check if we have key pressed or released
                if(key_pressed){                      //7'b set      //We active CTRL:
                    tracking_variable |= TRACK_CTRL; // we set TRACK_CTRL into tracking variable
                                                    // tracking variable=  0000 0000
                                                    // TRACK_CTRL=         0000 0001
                                                    // Doing operation |= -----------
                                                    // We get              0000 0001
                }else if(key_released) {
                    tracking_variable &= ~TRACK_CTRL; // we clear bit 0x01 from the tracking variable
                }
        break;

        case KEY_ALT_L:
                if(key_pressed){
                    tracking_variable |= TRACK_ALT;
                } else if (key_released) {
                    tracking_variable &= ~TRACK_ALT;
                }

        break;

        case KEY_ALT_R:
                if(key_pressed){
                    tracking_variable |= TRACK_ALT;
                } else if (key_released) {
                    tracking_variable &= ~TRACK_ALT;
                }

        break;

        case KEY_SHIFT_L:
                if(key_pressed){
                    tracking_variable |= TRACK_SHIFT;
                } else if (key_released) {
                    tracking_variable &= ~TRACK_SHIFT;
                }

        break;

        case KEY_SHIFT_R:
                if(key_pressed){
                    tracking_variable |= TRACK_SHIFT;
                } else if (key_released) {
                    tracking_variable &= ~TRACK_SHIFT;
                }

        break;

        case KEY_CAPS:
            if(key_pressed){
                tracking_variable ^= TRACK_CAPS;
                printf("keyboard: caps pressed");
            } else if (key_released) {
                printf("keyboard: caps released");
                break;
            }

        break;

        case KEY_NUMLOCK:
            if(key_pressed){
                tracking_variable |= TRACK_NUMLOCK;
            } else if (key_released) {
                break;
            }


        break;

        default:

        break;
    }

    if (key_pressed) {

        if (((tracking_variable & TRACK_SHIFT) != 0) && ((tracking_variable & TRACK_CAPS)  != 0))  {
            c = key_map_shiftCap[c];
        } else if (((tracking_variable & TRACK_SHIFT) != 0)) {
            c = key_map_shift[c];
        } else if ((tracking_variable & TRACK_CAPS)  != 0) {
            c = key_map_cap[c];
        }

        if (((tracking_variable) == 0)) {
            c = key_map_NSC[c];
        } else if(((tracking_variable & TRACK_NUMLOCK) != 0)) {
            c = key_map_numLock[c];
        }

        if(c!= KEY_NULL) {
            return c;
        }
    }

    return KEY_NULL;
}