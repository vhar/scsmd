/*
 * scsmd.h
 * Single Coil Machine Driver Interface
 * Copyright (c) 2016-2017, Valerie Valley RR https://sites.google.com/site/valerievalleyrr/
 */
#define CURRENTVERSION 102
#define CLOSE 0 // Turnout closed position signal level
#define THROW 1 // Turnout throwed position signal level
#define kDCC_INTERRUPT 0 // Pin 2 used as DCC Interrupt
#define EPPROM_POSITION 0 // EEPROM address for storing turnout position on DCC mode.
#define EEPROM_DECODER_CONTROL_MODE 1 // EEPROM address for storing the decoder control mode
#define EEPROM_DECODER_OUTPUT_INVERSE 2 // EEPROM address for storing the decoder control mode
#define EEPROM_DECODER_ADDRESS 9 //EEPROM address for storing the DCC Decoder address.
// Decoder address 1 contains Accessory addresses from 1 to 4
// Decoder address 2 contains Accessory addresses from 5 to 8, etc
#define EEPROM_DECODER_PULSE_TIME 39 //EEPROM address for storing the output pulse time.

#define PUSHBUTTON_HOLD_TIME 1500 // Push button hold time (in milliseconds) before switching mode.
#define LEARNING_MODE_WAIT 15000 // The DCC address waiting time (in milliseconds) in the Learning Mode.
// If no packet with an address has been received during this time, the decoder returns to the operating mode
#define LEARNING_BLINK_TIME 200 // LED blink time (in milliseconds) on Learning Mode
#define CHANGE_MODE_BLINK_TIME 1000 // Flashing time of the LED (in milliseconds) when the operating mode changes
#define INTERVAL_MILLIS 10 // Control Inputs debounce timeout

unsigned long learningMode = 0; // Timer flag for DCC address learning mode.
unsigned long pushbuttonHoldTime = 0; // Timer flag for pushbutton.
unsigned long ledFlag = 0; // Timer flag for LED control.
int  decoderAddress; // variable for operative storing current DCC decoder address
int decoderMode; // variable for operative storing current output control mode
int decoderInverse; // variable for operative current storing output polarity
unsigned long now; // variable for operative storing current millis
int flashes_number = 0; // number of flashes in LEARNING mode. For future use.
int flashes_count = 0; // count number of flashes in LEARNING mode. For future use.

/* 
 * Turnout structure
 * control_pin - pull up input used for turnout control in manual mode
 * enable_pin - connection to L293D "Enable" pin
 * red_pin - connection to L293D "Input 2"  pin
 * black_pin - connection to L293D "Input 1"  pin
 * pulse_time - pulse width in x*100ms on L293D output (10 = 1000ms)
 * motorRun - Timer flag for turnout switching process
 * controlState - current signal level on control_pin. 
 * controlChange - Timer flag for store last change signal lavel on control_pin.
 * controlMode - current output control mode. 0 = MANUAL, 1 = DCC.
 * polarityInverse - inverse output polarity. 0 = INVERSE, 1 = NORMAL.
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
  bool controlMode;
  bool polarityInverse;  
} Turnout;

/*
 * Turnouts structures array. Mode details see on scsm.h file
 * {control_pin, enable_pin, red_pin, black_pin, pulse_time, motorRun}
 */
Turnout turnouts[]={{18,6,16,17,0,0,0,0,1,1},{19,11,15,14,0,0,0,0,1,1},{20,9,10,12,0,0,0,0,1,1},{21,3,5,4,0,0,0,0,1,1}};

