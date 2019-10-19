/* @file Keypad.ino
|| @version 1.0
|| @author Alexandre da Silva
|| @contact lexrupy@gmail.com
|| @licence GPL v3
|| 
|| @description
|| | Reads keys of DIY Osciloscope keypad and send with USB as HID Keyboard
|| | The program was developed as simple as possible 
|| | so also beginners can understand the code, without
|| | use of registers or anything like that. Just some pointers ;)
|| |
|| # Target: Arduino Leonardo / Pro Micro with Atmega32U4 
|| #         Not work with Arduino boards with chips other than Atmega32U4 
|| #         With some adjustments may work with some Teensy Boards.

*/


#include <Keypad.h>
#include <Keyboard.h>

// This is a Virtual Size to get at least 22 Buttons
const byte ROWS = 4; //four rows
const byte COLS = 6; //six columns

// GENERAL CONTROLS
char GEN__NOT_USED__KEY = '$';
char GEN__RUN_STOP__KEY = 'r';
char GEN_SWTCH_MODE_KEY = 'm';
char GEN_SAMPLE_INC_KEY = 'q';
char GEN_SAMPLE_DEC_KEY = 'a';

//GRAPH CONTROLS
char GRAPH_ZI_YAXIS_KEY = KEY_UP_ARROW;
char GRAPH_ZO_YAXIS_KEY = KEY_DOWN_ARROW;
char GRAPH_ZI_XAXIS_KEY = KEY_LEFT_ARROW;
char GRAPH_ZO_XAXIS_KEY = KEY_RIGHT_ARROW;
char GRAPH_SWHD_MEN_KEY = ' ';
char GRAPH_WAV_FITY_KEY = 'f';

// CHANNEL 1 CONTROLS
char CH1_EN_DISABLE_KEY = '1';
char CH1_SW_INPUT_V_KEY = '7';
char CH1_SW_INPUT_P_KEY = '8';
char CH1_AUTO_VOLTS_KEY = '5';
char CH1_SW_ACDDOFF_KEY = 'z';
char CH1_SH_SAMPLES_KEY = 'x';
char CH1_SWHD_STATS_KEY = 'c';
char CH1_MOVE_YA_UP_KEY = 'i';
char CH1_MOVE_Y_DWN_KEY = 'k';

// CHANNEL 2 CONTROLS
char CH2_EN_DISABLE_KEY = '2';
char CH2_SW_INPUT_V_KEY = '9';
char CH2_SW_INPUT_P_KEY = '0';
char CH2_AUTO_VOLTS_KEY = '6';
char CH2_SW_ACDDOFF_KEY = 'v';
char CH2_SH_SAMPLES_KEY = 'b';
char CH2_SWHD_STATS_KEY = 'n';
char CH2_MOVE_YA_UP_KEY = 'o';
char CH2_MOVE_Y_DWN_KEY = 'l';


//TRIGGER CONTROLS
char TRIG_SWTC_CHAN_KEY = 't';
char TRIG_SWTC_SLPE_KEY = 's';
char TRIG_SWTC_MODE_KEY = 'g';
char TRIG_LEVEL_UP__KEY = 'y';
char TRIG_LEVEL_DN__KEY = 'h';


//HOLD CONTROLS
char CH1_VOLTS_AUTO_KEY = CH1_SW_INPUT_V_KEY;
char CH2_VOLTS_AUTO_KEY = CH2_SW_INPUT_V_KEY;
// KEY USED TO SEND AN "ENTER/RETURN" KEY/COMMAND
char GEN_SEND_ENTER_KEY = GEN__RUN_STOP__KEY;
// KEY USED TO SEND AN "ESCAPE" KEY/COMMAND
char GEN_SEND_ESCAP_KEY = GEN_SWTCH_MODE_KEY;

// DEFINE THE SYMBOLS/KEYSTROKES TO BE SENT WITH KEYBOARD
char hexaKeys[ROWS][COLS] = {
  {TRIG_SWTC_CHAN_KEY, GEN__NOT_USED__KEY, TRIG_SWTC_SLPE_KEY, GEN_SWTCH_MODE_KEY, GRAPH_SWHD_MEN_KEY, TRIG_SWTC_MODE_KEY},
  {CH2_EN_DISABLE_KEY, CH2_SH_SAMPLES_KEY, CH2_SW_ACDDOFF_KEY, CH2_SWHD_STATS_KEY, CH2_SW_INPUT_P_KEY, CH2_SW_INPUT_V_KEY},
  {GEN__NOT_USED__KEY, GEN_SAMPLE_DEC_KEY, GRAPH_WAV_FITY_KEY, GEN__RUN_STOP__KEY, GEN_SAMPLE_INC_KEY, GEN__NOT_USED__KEY},
  {CH1_EN_DISABLE_KEY, CH1_SH_SAMPLES_KEY, CH1_SW_ACDDOFF_KEY, CH1_SWHD_STATS_KEY, CH1_SW_INPUT_P_KEY, CH1_SW_INPUT_V_KEY},
};

byte rowPins[ROWS] = {6, 5, 4, 0}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {16, 14, 15, 9, 8, 7}; //connect to the column pinouts of the keypad

// Pots
short x_axisPin  = A0;
short y_axisPin  = A10;
short ch1_posPin = A1;
short ch2_posPin = A2;
short trg_lvlPin = A3;

short potLow   = 0;
short potHigh  = 1023;

short potDelayLow  = 50;
short potDelayHigh = 500;

// POT CONFIG
// MINUS AND PLUS ARE DEADZONE CONFIGURATION FOR YOUR POTS
// YOU WILL NEED TO CALIBRATE YOURS.
// TO CALIBRATE CREATE YOUR OWN SKETCH AND USE analogRead() TO SEE THE UPPER AND LOWER VALUE OF YOUR POT
// THEN YOUR CAN SEE THE VALUE AND ASSUME YOUR OWN VALUES FOR MINUS AND PLUS VALUES.
// ANY READ BELOW MINUS WILL SEND THE THE "LEFT" COMMAND AND ABOVE PLUS WILL SENT THE "RIGHT" COMMAND

// DEADZONE CONFIG FOR X AXIS ZOOM
short x_axis_potMinus = 575;
short x_axis_potPlus  = 895;
short x_axis_potMid   = (x_axis_potMinus + x_axis_potPlus) / 2;

// DEADZONE CONFIG FOR Y AXIS ZOOM
short y_axis_potMinus = 610;
short y_axis_potPlus  = 902;
short y_axis_potMid   = (y_axis_potMinus + y_axis_potPlus) / 2;

// DEADZONE CONFIG FOR CH1 POSITION
short ch1_potMinus = 690;
short ch1_potPlus  = 925;
short ch1_potMid   = (ch1_potMinus + ch1_potPlus) / 2;

// DEADZONE CONFIG FOR CH2 POSITION
short ch2_potMinus = 266;
short ch2_potPlus  = 725;
short ch2_potMid   = (ch2_potMinus + ch2_potPlus) / 2;


// DEADZONE CONFIG FOR TRIGGER LEVEL POSITION
short trg_potMinus = 320;
short trg_potPlus  = 760;
short trg_potMid   = (trg_potMinus + trg_potPlus) / 2;


unsigned long x_axis_ms  = 0;
unsigned long y_axis_ms  = 0;
unsigned long ch1_pos_ms = 0;
unsigned long ch2_pos_ms = 0;
unsigned long trg_lvl_ms = 0;


bool x_axis_send  = false;
bool y_axis_send  = false;
bool ch1_pos_send = false;
bool ch2_pos_send = false;
bool trg_lvl_send = false;

bool ch1_volts_auto_hold_state = false;
bool ch2_volts_auto_hold_state = false;

bool run_hold_state = false;
bool mode_hold_state = false;

//initialize an instance of class NewKeypad
Keypad scopeKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup(){
  Keyboard.begin();
  
  // ADJUST THE DEBOUNCE TIME FOR THE TYPE OF BUTTONS
  scopeKeypad.setDebounceTime(50);
  // CONFIGURE TIME TO THE BUTTON BE CONSIDERED HOLD
  scopeKeypad.setHoldTime(1500);
  // ATTACH THE EVENT LISTENER TO EXECUTE THE KEYPAD ACTIONS
  scopeKeypad.addEventListener(keypadEvent);
}
  
void loop(){
  // READ KEYPAD
  char key = scopeKeypad.getKey();
  
  // DEPENDING ON POLARITY OF CONNECTION FOR YOUR POTENTIOMETER YOU MAY NEED TO SWAP KEY POSITION
  // READ POTENTIOMETERS AND SEND THE KEYSTROKES

  // READ POT FOR X AXIS ZOOM
  readPot(&x_axisPin, &x_axis_ms, &x_axis_send, GRAPH_ZO_XAXIS_KEY, GRAPH_ZI_XAXIS_KEY, x_axis_potMid, x_axis_potMinus, x_axis_potPlus);
  
  // READ POT FOR Y AXIS ZOOM
  readPot(&y_axisPin, &y_axis_ms, &y_axis_send, GRAPH_ZI_YAXIS_KEY, GRAPH_ZO_YAXIS_KEY, y_axis_potMid, y_axis_potMinus, y_axis_potPlus);
  
  // READ POT FOR MOVE CHANNEL 1 POSITION UP AND DOWN ON SCREEN
  readPot(&ch1_posPin, &ch1_pos_ms, &ch1_pos_send, CH1_MOVE_YA_UP_KEY, CH1_MOVE_Y_DWN_KEY, ch1_potMid, ch1_potMinus, ch1_potPlus);
  
  // READ POT FOR MOVE CHANNEL 2 POSITION UP AND DOWN ON SCREEN
  readPot(&ch2_posPin, &ch2_pos_ms, &ch2_pos_send, CH2_MOVE_YA_UP_KEY, CH2_MOVE_Y_DWN_KEY, ch2_potMid, ch2_potMinus, ch2_potPlus);
  
  // READ POT FOR MOVE TRIGGER LEVEL POSITION UP AND DOWN ON SCREEN
  readPot(&trg_lvlPin, &trg_lvl_ms, &trg_lvl_send, TRIG_LEVEL_UP__KEY, TRIG_LEVEL_DN__KEY, trg_potMid, trg_potMinus, trg_potPlus);
  
}


void readPot(short *pin, unsigned long *timer, bool *vsend, char valLeft, char valRight, short mid, short minus, short plus) {
  unsigned long ms = millis();
  // FIRST ASSUME THE INITIAL VALUE AS OF MIDPOINT
  int val = mid;
  bool up = false;
  // READ THE ANALOG INPUT
  int pinVal = analogRead(*pin);
  // IF VALUE IS BELOW MINUS DEADZONE, PREPARE FOR "LEFT" COMAND
  if (pinVal <= minus ) {
    val = map(pinVal, potLow, minus, potDelayLow, potDelayHigh);
  // ELSE IF VALUE IS ABOVE PLUS DEADZONE, PREPARE FOR "RIGHT" COMMAND
  } else if (pinVal >= plus) {
    val = map(pinVal, potHigh, plus, potDelayLow, potDelayHigh);
    up = true;
  }
  // CALCULATE THE KEYPRESS SPEED AND SEND THE KEY ASSOCIATED WITH "UP" OR "DOWN" COMMAND
  if (val != mid) {
    if ((ms - *timer) >= val ) {
      *timer = ms;
      if (*vsend) {
        *vsend = false;
      } else {
        *vsend = true;
      }
      if (*vsend) {
        // SEND THE "RIGHT" COMMAND IF POT IS IN UP POSITION, OTHERWISE SEND THE "LEFT" COMMAND
        if (up) {
          Keyboard.write(valRight);
        } else {
          Keyboard.write(valLeft);
        }
      }
    }
  } else {
    // Update the timer
    *timer = ms;
  }
}

void sendKey(char key) {
  Keyboard.write(key);
}

// Taking care of some special events.
void keypadEvent(KeypadEvent key){
    switch (scopeKeypad.getState()){
//    case PRESSED:
//        break;
    case RELEASED:
        // SPECIAL FUNCTION FOR KEY VOLTS/AUTO ON CHANNELS NEED TO BE EVALUATED
        // IF BUTTON IS HOLD EXECUTE AUTO FUNCTION, ELSE
        // EXECUTE VOLTS FUNCTION
        if (key == CH1_VOLTS_AUTO_KEY) {
          if (ch1_volts_auto_hold_state) {
            ch1_volts_auto_hold_state = false;
          } else {
            // IF THE BUTTON WAS NOT HOLD, JUST SEND THE VOLTS KEYSTROKE
            Keyboard.write(key);
          }
        } else if (key == GEN__RUN_STOP__KEY) {
          if (run_hold_state) {
            run_hold_state = false;
          } else {
            Keyboard.write(key);
          }
        }  else if (key == GEN_SWTCH_MODE_KEY) {
          if (mode_hold_state) {
            mode_hold_state = false;
          } else {
            Keyboard.write(key);
          }
        } else if (key == CH2_VOLTS_AUTO_KEY) {
          if (ch2_volts_auto_hold_state) {
            ch2_volts_auto_hold_state = false;
          } else {
            // IF THE BUTTON WAS NOT HOLD, JUST SEND THE VOLTS KEYSTROKE
            Keyboard.write(key);
          }
        } else {
          // SEND THE PRESSED KEY
          Keyboard.write(key);
        }
        break;
    case HOLD:
        if (key == CH1_VOLTS_AUTO_KEY) {
            // CHECK CURRENT HOLD STATE FOR CHANNEL 1 VOLTS/AUTO
            if (!ch1_volts_auto_hold_state) {
              // IF WAS NOT HOLD, SEND AUTO COMMAND
              sendKey(CH1_AUTO_VOLTS_KEY);
              // SET HOLD STATE
              ch1_volts_auto_hold_state = true;
            }
        }
        if (key == CH2_VOLTS_AUTO_KEY) {
            if (!ch2_volts_auto_hold_state) {
              // IF WAS NOT HOLD, SEND AUTO COMMAND
              sendKey(CH2_AUTO_VOLTS_KEY);
              // SET HOLD STATE
              ch2_volts_auto_hold_state = true;
            }
        }
        if (key == GEN_SEND_ENTER_KEY) {
            if (!run_hold_state) {
              // IF WAS NOT HOLD, SEND AUTO COMMAND
              sendKey(KEY_RETURN);
              // SET HOLD STATE
              run_hold_state = true;
            }
        }
        if (key == GEN_SEND_ESCAP_KEY) {
            if (!mode_hold_state) {
              // IF WAS NOT HOLD, SEND AUTO COMMAND
              sendKey(KEY_ESC);
              // SET HOLD STATE
              mode_hold_state = true;
            }
        }
        break;
    }
}
