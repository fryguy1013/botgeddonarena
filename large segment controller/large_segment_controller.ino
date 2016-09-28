
#include "settings.h"
#include <Wire.h>
#include <EEPROM.h>


//    -  A
// F | | B
//    _  G
// E | | C
//    _  D

//This is the combined array that contains all the segment configurations for many different characters and symbols
const uint8_t characterArray[] = {
//  ABCDEFG  Segments
  0b1111110, // 0
  0b0110000, // 1
  0b1101101, // 2
  0b1111001, // 3
  0b0110011, // 4
  0b1011011, // 5
  0b1011111, // 6
  0b1110000, // 7
  0b1111111, // 8
  0b1111011, // 9
  0b1110111, // 10  "A"
  0b0011111, // 11  "B"
  0b1001110, // 12  "C"
  0b0111101, // 13  "D"
  0b1001111, // 14  "E"
  0b1000111, // 15  "F"
  0b0000000, // 16  NO DISPLAY
  0b0000000, // 17  NO DISPLAY
  0b0000000, // 18  NO DISPLAY
  0b0000000, // 19  NO DISPLAY
  0b0000000, // 20  NO DISPLAY
  0b0000000, // 21  NO DISPLAY
  0b0000000, // 22  NO DISPLAY
  0b0000000, // 23  NO DISPLAY
  0b0000000, // 24  NO DISPLAY
  0b0000000, // 25  NO DISPLAY
  0b0000000, // 26  NO DISPLAY
  0b0000000, // 27  NO DISPLAY
  0b0000000, // 28  NO DISPLAY
  0b0000000, // 29  NO DISPLAY
  0b0000000, // 30  NO DISPLAY
  0b0000000, // 31  NO DISPLAY
  0b0000000, // 32 ' '
  0b0000000, // 33 '!'  NO DISPLAY
  0b0100010, // 34 '"'
  0b0000000, // 35 '#'  NO DISPLAY
  0b0000000, // 36 '$'  NO DISPLAY
  0b0000000, // 37 '%'  NO DISPLAY
  0b0000000, // 38 '&'  NO DISPLAY
  0b0100000, // 39 '''
  0b1001110, // 40 '('
  0b1111000, // 41 ')'
  0b0000000, // 42 '*'  NO DISPLAY
  0b0000000, // 43 '+'  NO DISPLAY
  0b0000100, // 44 ','
  0b0000001, // 45 '-'
  0b0000000, // 46 '.'  NO DISPLAY
  0b0000000, // 47 '/'  NO DISPLAY
  0b1111110, // 48 '0'
  0b0110000, // 49 '1'
  0b1101101, // 50 '2'
  0b1111001, // 51 '3'
  0b0110011, // 52 '4'
  0b1011011, // 53 '5'
  0b1011111, // 54 '6'
  0b1110000, // 55 '7'
  0b1111111, // 56 '8'
  0b1111011, // 57 '9'
  0b0000000, // 58 ':'  NO DISPLAY
  0b0000000, // 59 ';'  NO DISPLAY
  0b0000000, // 60 '<'  NO DISPLAY
  0b0000000, // 61 '='  NO DISPLAY
  0b0000000, // 62 '>'  NO DISPLAY
  0b0000000, // 63 '?'  NO DISPLAY
  0b0000000, // 64 '@'  NO DISPLAY
  0b1110111, // 65 'A'
  0b0011111, // 66 'B'
  0b1001110, // 67 'C'
  0b0111101, // 68 'D'
  0b1001111, // 69 'E'
  0b1000111, // 70 'F'
  0b1011110, // 71 'G'
  0b0110111, // 72 'H'
  0b0110000, // 73 'I'
  0b0111000, // 74 'J'
  0b0000000, // 75 'K'  NO DISPLAY
  0b0001110, // 76 'L'
  0b0000000, // 77 'M'  NO DISPLAY
  0b0010101, // 78 'N'
  0b1111110, // 79 'O'
  0b1100111, // 80 'P'
  0b1110011, // 81 'Q'
  0b0000101, // 82 'R'
  0b1011011, // 83 'S'
  0b0001111, // 84 'T'
  0b0111110, // 85 'U'
  0b0000000, // 86 'V'  NO DISPLAY
  0b0000000, // 87 'W'  NO DISPLAY
  0b0000000, // 88 'X'  NO DISPLAY
  0b0111011, // 89 'Y'
  0b0000000, // 90 'Z'  NO DISPLAY
  0b1001110, // 91 '['
  0b0000000, // 92 '\'  NO DISPLAY
  0b1111000, // 93 ']'
  0b0000000, // 94 '^'  NO DISPLAY
  0b0001000, // 95 '_'
  0b0000010, // 96 '`'
  0b1110111, // 97 'a' SAME AS CAP
  0b0011111, // 98 'b' SAME AS CAP
  0b0001101, // 99 'c'
  0b0111101, // 100 'd' SAME AS CAP
  0b1101111, // 101 'e'
  0b1000111, // 102 'f' SAME AS CAP
  0b1011110, // 103 'g' SAME AS CAP
  0b0010111, // 104 'h'
  0b0010000, // 105 'i'
  0b0111000, // 106 'j' SAME AS CAP
  0b0000000, // 107 'k'  NO DISPLAY
  0b0110000, // 108 'l'
  0b0000000, // 109 'm'  NO DISPLAY
  0b0010101, // 110 'n' SAME AS CAP
  0b0011101, // 111 'o'
  0b1100111, // 112 'p' SAME AS CAP
  0b1110011, // 113 'q' SAME AS CAP
  0b0000101, // 114 'r' SAME AS CAP
  0b1011011, // 115 's' SAME AS CAP
  0b0001111, // 116 't' SAME AS CAP
  0b0011100, // 117 'u'
  0b0000000, // 118 'b'  NO DISPLAY
  0b0000000, // 119 'w'  NO DISPLAY
  0b0000000, // 120 'x'  NO DISPLAY
  0b0000000, // 121 'y'  NO DISPLAY
  0b0000000, // 122 'z'  NO DISPLAY
  0b0000000, // 123 '0b'  NO DISPLAY
  0b0000000, // 124 '|'  NO DISPLAY
  0b0000000, // 125 ','  NO DISPLAY
  0b0000000, // 126 '~'  NO DISPLAY
  0b0000000, // 127 'DEL'  NO DISPLAY
};

struct dataBuffer
{
  unsigned char data[BUFFER_SIZE];  // THE data buffer
  unsigned int head;  // store new data at this index
  unsigned int tail;  // read oldest data from this index
}
buffer;  // our data buffer is creatively named - buffer

// Struct for 4-digit, 7-segment display
// Stores display value (digits),  decimal status (decimals) for each digit, and cursor for overall display
struct display
{
  char digits[3];
  unsigned char decimals;
  unsigned char cursor;
}
display;  // displays be displays

unsigned char commandMode = 0;  // Used to indicate if a commandMode byte has been received


byte segmentData = 2;
byte segmentClock = 3;
byte segmentLatch = 4;

// setupTWI(): initializes I2C (err TWI! TWI! TWI!, can't bang that into my head enough)
// I'm using the rock-solid Wire library for this. We'll initialize TWI, setup the address,
// and tell it what interrupt function to jump to when data is received.
void setupTWI()
{
  unsigned char twiAddress;

  twiAddress = EEPROM.read(TWI_ADDRESS_ADDRESS);  // read the TWI address from

  if ((twiAddress == 0) || (twiAddress > 0x7F))
  { // If the TWI address is invalid, use a default address
    twiAddress = TWI_ADDRESS_DEFAULT;
    EEPROM.write(TWI_ADDRESS_ADDRESS, TWI_ADDRESS_DEFAULT);
  }

  Wire.begin(twiAddress);  // Initialize Wire library as slave at twiAddress address
  Wire.onReceive(twiReceive);  // setup interrupt routine for when data is received
}

// seutpTimer(): Set up timer 1, which controls interval reading from the buffer
void setupTimer()
{
  // Timer 1 is se to CTC mode, 16-bit timer counts up to 0xFF
  TCCR1B = (1<<WGM12) | (1<<CS10);
  OCR1A = 0x00FF;
  TIMSK1 = (1<<OCIE1A);  // Enable interrupt on compare
}

void setup()
{
  pinMode(segmentClock, OUTPUT);
  pinMode(segmentData, OUTPUT);
  pinMode(segmentLatch, OUTPUT);

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, LOW);
  digitalWrite(segmentLatch, LOW);

  setupTWI();
  setupTimer();

  display.digits[0] = '1';
  display.digits[1] = '2';
  display.digits[2] = '3';
}

void loop()
{
  displaySegments();
  delay(100);
}

//Takes a number and displays 2 numbers. Displays absolute value (no negatives)
void displaySegments()
{
  for (byte x = 0; x < 3; x++)
  {
    byte segments = display.digits[2 - x];

    if (segments & 0x80) // raw mode
    {
      segments = segments & 0x7f;
    }
    else
    {
      segments = characterArray[segments];
    }

    postSegment(segments);
  }

  //Latch the current segment data
  digitalWrite(segmentLatch, LOW);
  digitalWrite(segmentLatch, HIGH); //Register moves storage register on the rising edge of RCK
}

void postSegment(byte segments)
{
  //    -  A
  //   / / F/B
  //    -  G
  //   / / E/C
  //    -. D/DP

const byte a = 1<<6;
const byte b = 1<<5;
const byte c = 1<<4;
const byte d = 1<<3;
const byte e = 1<<2;
const byte f = 1<<1;
const byte g = 1<<0;
const byte dp = 1<<7;

  // output in order DP B C D E G F A

  //Clock these bits out to the drivers
  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, segments & dp);
  digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, segments & b);
  digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, segments & c);
  digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, segments & d);
  digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, segments & e);
  digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, segments & g);
  digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, segments & f);
  digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK

  digitalWrite(segmentClock, LOW);
  digitalWrite(segmentData, segments & a);
  digitalWrite(segmentClock, HIGH); //Data transfers to the register on the rising edge of SRCK
}



// The display data is updated on a Timer interrupt
ISR(TIMER1_COMPA_vect)
{
  noInterrupts();

  // if head and tail are not equal, there's data to be read from the buffer
  if (buffer.head != buffer.tail)
    updateBufferData();  // updateBufferData() will update the display info, or peform special commands

  interrupts();
}

void twiReceive(int rxCount)
{
  while (Wire.available()) // Do this while data is available in Wire buffer
  {
    unsigned int i = (buffer.head + 1) % BUFFER_SIZE;  // read buffer head position and increment
    unsigned char c = Wire.read();  // Read data byte into c, from Wire data buffer

    if (i != buffer.tail)  // As long as the buffer isn't full, we can store the data in buffer
    {
      buffer.data[buffer.head] = c;  // Store the data into the buffer's head
      buffer.head = i;  // update buffer head, since we stored new data
    }
  }
}

// updateBufferData(): This beast of a function is called by the Timer 1 ISR if there is new data in the buffer. 
// If the data controls display data, that'll be updated.
// If the data relates to a command, commandmode will be set accordingly or a command 
// will be executed from this function.
void updateBufferData()
{
  // First we read from the oldest data in the buffer
  unsigned char c = buffer.data[buffer.tail];
  buffer.tail = (buffer.tail + 1) % BUFFER_SIZE;  // and update the tail to the next oldest

  // if the last byte received wasn't a command byte (commandMode=0)
  // and if the data is displayable (0-0x76 or 0x78), the display will be updated
  if ((commandMode == 0) && ((c < 0x76) || (c == 0x78)))
  {
    display.digits[display.cursor] = c;  // just store the read data into the cursor-active digit
    display.cursor = ((display.cursor + 1) % 3);  // Increment cursor, set back to 0 if necessary
  }
  else if ((c == RESET_CMD) && (!commandMode))  // If the received char is the reset command
  {
    for(int i = 0 ; i < 3; i++)
      display.digits[i] = 'x';  // clear all digits
    display.decimals = 0;  // clear all decimals
    display.cursor = 0;  // reset the cursor
  }
  else if (commandMode != 0)  // Otherwise, if data is non-displayable and we're in a commandMode
  {
    switch (commandMode)
    {
    case DECIMAL_CMD:  // Decimal setting mode
      display.decimals = c;  // decimals are set by one byte
      break;
    case CURSOR_CMD:  // Set the cursor
      if (c <= 3)  // Limited error checking, if >3 cursor command will have no effect
        display.cursor = c;  // Update the cursor value
      break;
    case TWI_ADDRESS_CMD:  // Set the I2C Address
      EEPROM.write(TWI_ADDRESS_ADDRESS, c); // Update the EEPROM value
      setupTWI(); //Checks to see if I2C address is valid and begins I2C
      break;
    case DIGIT1_CMD:  // Single-digit control for digit 1
      display.digits[0] = c | 0x80;  // set msb to indicate single digit control mode
      break;
    case DIGIT2_CMD:  // Single-digit control for digit 2
      display.digits[1] = c | 0x80;
      break;
    case DIGIT3_CMD:  // Single-digit control for digit 3
      display.digits[2] = c | 0x80;
      break;
    }
    // Leaving commandMode 
    // !!! If the commandMode isn't a valid command, we'll leave command mode, should be checked below?
    commandMode = 0;
  }
  else  // Finally, if we weren't in command mode, if the byte isn't displayable, we'll enter command mode
  {
    commandMode = c;  // which command mode is reflected by value of commandMode
  }
}

