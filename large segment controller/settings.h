
#include "Arduino.h"

//Bit locations for the decimal, apostrophe and colon control
//From https://github.com/sparkfun/Serial7SegmentDisplay/wiki/Special-Commands#wiki-decimal
#define DECIMAL3    2
#define DECIMAL2    1
#define DECIMAL1    0

#define MODE_DATA    0
#define MODE_ANALOG  1
#define MODE_COUNTER 2

const int TWI_ADDRESS_DEFAULT = 0x61;

//Internal EEPROM locations for the user settings
const unsigned char TWI_ADDRESS_ADDRESS = 2;

/* Command Modes */
const unsigned char RESET_CMD         = 0x76;
const unsigned char DECIMAL_CMD       = 0x77;
const unsigned char BLANK_CMD         = 0x78;  // 'x'
const unsigned char CURSOR_CMD        = 0x79;  // !!! NEW
const unsigned char DIGIT1_CMD        = 0x7B;
const unsigned char DIGIT2_CMD        = 0x7C;
const unsigned char DIGIT3_CMD        = 0x7D;
const unsigned char DIGIT4_CMD        = 0x7E;
const unsigned char TWI_ADDRESS_CMD   = 0x80;  // !!! NEW
const unsigned char FACTORY_RESET_CMD = 0x81;  // !!! NEW

const int BUFFER_SIZE = 64;


