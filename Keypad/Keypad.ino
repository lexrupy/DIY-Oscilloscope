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

//#define DEBUG

#include <Keypad.h>
#ifndef DEBUG
#include <Keyboard.h>
#endif

// This is a Virtual Size to get at least 22 Buttons
const byte ROWS = 4; //four rows
const byte COLS = 6; //six columns

char run_key = 'r';
char mode_key = 'm';

char ch1_volts_auto_key = '7';
char ch2_volts_auto_key = '8';
char ch1_auto_key = '5';
char ch2_auto_key = '6';

// DEFINE THE SYMBOLS/KEYSTROKES TO BE SENT WITH KEYBOARD
char hexaKeys[ROWS][COLS] = {
  {'t','$','s',mode_key,' ','g'},
  {'2','b','v','n','0','9'},
  {'$','a','f',run_key,'q','$'},
  {'1','x','z','c','8','7'},
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

short x_axis_potMinus = 575;
short x_axis_potPlus  = 895;
short x_axis_potMid   = (x_axis_potMinus + x_axis_potPlus) / 2;



short y_axis_potMinus = 610;
short y_axis_potPlus  = 902;
short y_axis_potMid   = (y_axis_potMinus + y_axis_potPlus) / 2;



short ch1_potMinus = 690;
short ch1_potPlus  = 925;
short ch1_potMid   = (ch1_potMinus + ch1_potPlus) / 2;


short ch2_potMinus = 266;
short ch2_potPlus  = 725;
short ch2_potMid   = (ch2_potMinus + ch2_potPlus) / 2;



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
  #ifdef DEBUG
  Serial.begin(9600);
  #endif
  #ifndef DEBUG
  Keyboard.begin();
  #endif
  
  // ADJUST THE DEBOUNCE TIME FOR THE TYPE OF BUTTONS
  scopeKeypad.setDebounceTime(50);
  // CONFIGURE TIME TO THE BUTTON BE CONSIDERED HOLD
  scopeKeypad.setHoldTime(1500);
  // ATTACH THE EVENT LISTENER TO EXECUTE THE KEYPAD ACTIONS
  scopeKeypad.addEventListener(keypadEvent);
}
  
void loop(){
  char key = scopeKeypad.getKey();
  // READ POTENTIOMETERS AND SEND THE KEYSTROKES
  #ifndef DEBUG
  readPot(&x_axisPin, &x_axis_ms, &x_axis_send, KEY_RIGHT_ARROW, KEY_LEFT_ARROW, x_axis_potMid, x_axis_potMinus, x_axis_potPlus);
  readPot(&y_axisPin, &y_axis_ms, &y_axis_send, KEY_UP_ARROW, KEY_DOWN_ARROW, y_axis_potMid, y_axis_potMinus, y_axis_potPlus);
  #endif
  #ifdef DEBUG
  readPot(&x_axisPin, &x_axis_ms, &x_axis_send, '>', '<', x_axis_potMid, x_axis_potMinus, x_axis_potPlus);
  readPot(&y_axisPin, &y_axis_ms, &y_axis_send, '+', '-', y_axis_potMid, y_axis_potMinus, y_axis_potPlus);  
  #endif
  readPot(&ch1_posPin, &ch1_pos_ms, &ch1_pos_send, 'i', 'k', ch1_potMid, ch1_potMinus, ch1_potPlus);
  readPot(&ch2_posPin, &ch2_pos_ms, &ch2_pos_send, 'o', 'l', ch2_potMid, ch2_potMinus, ch2_potPlus);
  readPot(&trg_lvlPin, &trg_lvl_ms, &trg_lvl_send, 'y', 'h', trg_potMid, trg_potMinus, trg_potPlus);
  // Serial.println();
}


void readPot(short *pin, unsigned long *timer, bool *vsend, char valLeft, char valRight, short mid, short minus, short plus) {
  unsigned long ms = millis();
  int val = mid;
  bool up = false;
  int pinVal    = analogRead(*pin);
//  Serial.print(pinVal);
//  Serial.print("  -  ");
  if (pinVal <= minus ) {
    val = map(pinVal, potLow, minus, potDelayLow, potDelayHigh);
  } else if (pinVal >= plus) {
    val = map(pinVal, potHigh, plus, potDelayLow, potDelayHigh);
    up = true;
  }

  if (val != mid) {
    if ((ms - *timer) >= val ) {
      *timer = ms;
      if (*vsend) {
        *vsend = false;
      } else {
        *vsend = true;
      }
      if (*vsend) {
        if (up) {
          #ifndef DEBUG
          Keyboard.write(valRight);
          #endif
          #ifdef DEBUG
          Serial.println(valRight);
          #endif
        } else {
          #ifndef DEBUG
          Keyboard.write(valLeft);
          #endif
          #ifdef DEBUG
          Serial.println(valLeft);
          #endif
        }
      }
    }
  } else {
    // Update the timer
    *timer = ms;
  }
}

void sendKey(char key) {
  #ifndef DEBUG
  Keyboard.write(key);
  #endif
  #ifdef DEBUG
  if (key == KEY_RETURN) {
    Serial.println("[RETURN]");
  } else if (key == KEY_ESC) {
    Serial.println("[ESC]");
  } else {
    Serial.println(key);
  }
  #endif
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
        if (key == ch1_volts_auto_key) {
          if (ch1_volts_auto_hold_state) {
            ch1_volts_auto_hold_state = false;
          } else {
            // IF THE BUTTON WAS NOT HOLD, JUST SEND THE VOLTS KEYSTROKE
            #ifndef DEBUG
            Keyboard.write(key);
            #endif
            #ifdef DEBUG
            Serial.println(key);
            #endif
          }
        } else if (key == run_key) {
          if (run_hold_state) {
            run_hold_state = false;
          } else {
            #ifndef DEBUG
            Keyboard.write(key);
            #endif
            #ifdef DEBUG
            Serial.println(key);
            #endif
          }
        }  else if (key == mode_key) {
          if (mode_hold_state) {
            mode_hold_state = false;
          } else {
            #ifndef DEBUG
            Keyboard.write(key);
            #endif
            #ifdef DEBUG
            Serial.println(key);
            #endif
          }
        } else if (key == ch2_volts_auto_key) {
          if (ch2_volts_auto_hold_state) {
            ch2_volts_auto_hold_state = false;
          } else {
            // IF THE BUTTON WAS NOT HOLD, JUST SEND THE VOLTS KEYSTROKE
            #ifndef DEBUG
            Keyboard.write(key);
            #endif
            #ifdef DEBUG
            Serial.println(key);
            #endif
          }
        } else {
          // SEND THE PRESSED KEY
          #ifndef DEBUG
          Keyboard.write(key);
          #endif
          #ifdef DEBUG
          Serial.println(key);
          #endif
        }
        break;
    case HOLD:
        if (key == ch1_volts_auto_key) {
            // CHECK CURRENT HOLD STATE FOR CHANNEL 1 VOLTS/AUTO
            if (!ch1_volts_auto_hold_state) {
              // IF WAS NOT HOLD, SEND AUTO COMMAND
              sendKey(ch1_auto_key);
              // SET HOLD STATE
              ch1_volts_auto_hold_state = true;
            }
        }
        if (key == ch2_volts_auto_key) {
            if (!ch2_volts_auto_hold_state) {
              // IF WAS NOT HOLD, SEND AUTO COMMAND
              sendKey(ch2_auto_key);
              // SET HOLD STATE
              ch2_volts_auto_hold_state = true;
            }
        }
        if (key == run_key) {
            if (!run_hold_state) {
              // IF WAS NOT HOLD, SEND AUTO COMMAND
              sendKey(KEY_RETURN);
              // SET HOLD STATE
              run_hold_state = true;
            }
        }
        if (key == mode_key) {
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
