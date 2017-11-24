/*
 * scsm.h
 * Single Coil Machine Driver Interface
 * Copyright (c) 2016-2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 */
#define CURRENTVERSION 101
#define CLOSE 0 // Turnout closed position signal level
#define THROW 1 // Turnout throwed position signal level
#define kDCC_INTERRUPT 0 // Pin 2 used as DCC Interrupt
#define EEPROM_MODE 0 // EEPROM address for storing the decoder control mode
#define EPPROM_POSITION 1 // EEPROM starting address for storing turnout position on DCC mode.
// EPPROM_POSITION for turnout #1, EPPROM_POSITION+1 for turnout #2, EPPROM_POSITION+2 for turnout #3, EPPROM_POSITION+3 for turnout #4
#define EEPROM_ADDRESS 5 //EEPROM address for storing the DCC Decoder address.
// Decoder address 1 contains Accessory addresses from 1 to 4
// Decoder address 2 contains Accessory addresses from 5 to 8, etc
#define PUSHBUTTON_HOLD_TIME 1500 // Push button hold time (in milliseconds) before switching mode.
#define LEARNING_MODE_WAIT 30000 // The DCC address waiting time (in milliseconds) in the Learning Mode.
// If no packet with an address has been received during this time, the decoder returns to the operating mode
#define LEARNING_BLINK_TIME 200 // LED blink time (in milliseconds) on Learning Mode
#define CHANGE_MODE_BLINK_TIME 500 // Flashing time of the LED (in milliseconds) when the operating mode changes
#define DCC_MODE_LEDCONTROL_STATE 1 // If set 1, controlling LED on pin LEDCONTROL
// is burns if operating mode is DCC and is off if operating mode is MANUAL
// if set to 0, it will be backwards
#define INTERVAL_MILLIS 10 // Control Inputs debounce timeout

unsigned long learningMode = 0; // Timer flag for DCC address learning mode.
unsigned long pushbuttonHoldTime = 0; // Timer flag for pushbutton.
unsigned long ledFlag = 0; // Timer flag for LED control.
int  decoderAddress; // variable for operative storing current DCC decoder address
bool decoderMode; // variable for operative storing current decoder mode
unsigned long now; // variable for operative storing current millis
int flashes_number = 0; // number of flashes in LEARNING mode. For future use.
int flashes_count = 0; // count number of flashes in LEARNING mode. For future use.

/* 
 * Turnout structure
 * control_pin - pull up input used for turnout control in manual mode
 * enable_pin - connection to L293D "Enable" pin
 * red_pin - connection to L293D "Input 2"  pin
 * black_pin - connection to L293D "Input 1"  pin
 * pulse_time - pulse width in milliseconds on L293D output
 * motorRun - Timer flag for turnout switching process
 */
typedef struct
{
  int control_pin;
  int enable_pin;
  int red_pin;
  int black_pin;
  int pulse_time;
  unsigned long motorRun;
  bool controlState;
  unsigned long controlChange;
} Turnout;

/*
 * Turnouts structures array. Mode details see on scsm.h file
 * {control_pin, enable_pin, red_pin, black_pin, pulse_time, motorRun}
 */
Turnout turnouts[]={{18,6,16,17,1000,0,0,0},{19,11,15,14,1000,0,0,0},{20,9,10,12,1000,0,0,0},{21,3,5,4,1000,0,0,0}};

