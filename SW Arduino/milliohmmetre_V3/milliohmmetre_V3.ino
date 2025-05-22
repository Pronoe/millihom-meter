//=======================================================================/
// Software for milliohmmeter board with Arduino UNO                     /
// the board uses an INA199 IC for voltage amplification with low offset /
// and a constant current source of 10.55 mA powered by Arduino Vcc (5V) /
// The Arduino UNO board is fitted with a LCD 2x16 characters LCD dsplay /
// Software requirements:                                                /
// - continuous resistance measurement and result display at 1s rate     /
// - zero compensation when "SELECT" key depressed                       /
// - scale factor calibration when "SELECT" key depressed and reference  /
//   value of 9.09 ohm +/- 1% is connected to the input                  /
// - calibration data saved in EEPROM                           /
//=======================================================================/
// Author: Patrick SOUTY (Pronoe)
// Date of creation: 02/04/2025
// under GNU General Public License v3.0
// Versions:
// 1.0 - 02/04/2025 - initial version
// 2.0 - 08/05/2025 - use of external buttons following hardware integration
// 2.1 - 14/05/2025 - cleaning of code before publication and minor corrections
// 3.0 - 22/05/2025 - IIR filter mode added + double ADC measurement + measurement rate selection
//================================================================================================/

String SW_name = "Milliohm";
String SW_version = "V3.0";
#include <LiquidCrystal.h>  // using LiquidCrystal library
#include <EEPROM.h>         // using EEPROM library

// define EEPROM addresses used for SF1 and ADC_0 storage
#define SF1_ADD 0
#define ADC_0_ADD 4

#define SF1_default 9258  // default scale factor value in µohm/bit
// define analog input for V(R) measurement
#define Vin A1            // use A1 for INA199 output voltage measurement
#define polling_rate 200  // polling rate of buttons and switches in ms
// define variables used for resistance measurement
int meas_rate = 1000;                                        // delay between measurements in ms
int ADC_read;                                                // used for ADC measurement storage
int SF1;                                                     // scale factor in µohm/bit
int ADC_0;                                                   // ADC value for zero ohm input (i.e. offset value)
int Rref = 9090;                                             // calibration resistance in milliohm
int Rmeas;                                                   // measured resistance value in mohm
long Rmeas_long;                                             // for 32 bits computation of Rmeas
long Rmeas_filtered;                                         // filtered value of Rmeas on 32 bits
bool filter_on;                                              // filter mode ON or OFF
int ADC_min = int((long(Rref) * 965) / long(SF1_default));   // ADC lowest value for scale factor calibration (nominal value -3.5%)
int ADC_max = int((long(Rref) * 1035) / long(SF1_default));  // ADC highest value for scale factor calibration (nominal value +3.5%)
int ADC_0_max = 11;                                          // ADC highest value for zero compensation
int coeff_filt = 100;                                        // actual value of order 1 IIR filter multiplied by 1000 (i.e. 0.1 for the actual value)
int i;                                                       // index for loops
unsigned long time;                                          // for time management of adc sampling
unsigned long polling_time;                                  // for time management of buttns and switch polling
bool keyProcessed;                                           // used to avoid multiple processing of a single key pressed
bool test = false;
int ADC_test = 2;  // used for test
String MyString;
String Blank = "                ";  // used to clear a full LCD line
String current_rate = "slow/";
String current_filter = "filter off";
// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
// define some values used by the panel and buttons
#define pushButton1 A2   // gray wire - note: A2, A3 and A4 used as digital inputs
#define pushButton2 A3   // yellow wire
#define switchButton A4  // green wire
int calSelect = 0;
int validateSelect = 0;  // push button state - reserved for future use
int modeState = 0;       // stat of the mode button - reserved for future use

// reads HW push buttons and switch subroutine
void read_HW_buttons() {
  calSelect = !digitalRead(pushButton1);
  validateSelect = !digitalRead(pushButton2);
  modeState = !digitalRead(switchButton);
  if (test) {
    Serial.print("buttons state : ");
    Serial.print(calSelect);
    Serial.print(" ");
    Serial.print(validateSelect);
    Serial.print(" ");
    Serial.println(modeState);
  }
}

void setup() {
  // read calibration values in EEPROM
  Serial.begin(115200);
  ADC_0 = -1;                // negative value states that zero compensation has not been done
  EEPROM.get(SF1_ADD, SF1);  // read SF1 value in EEPROM
  if ((SF1 > 9700) || (SF1 < 8800)) {
    SF1 = SF1_default;
  }
  // input buttons configuration
  pinMode(pushButton1, INPUT_PULLUP);
  pinMode(pushButton2, INPUT_PULLUP);
  pinMode(switchButton, INPUT_PULLUP);
  // LCD display init
  lcd.begin(16, 2);  // start the LCD library
  lcd.display();
  lcd.setCursor(0, 0);
  MyString = SW_name + "  " + SW_version;
  Serial.println(MyString);
  Serial.println("Program started ...");
  lcd.print(MyString);  // print SW information
  lcd.setCursor(0, 1);
  lcd.print("set zero & "
            "SELECT"
            "");

  time = millis() + meas_rate;             // save next time to make measurement
  polling_time = millis() + polling_rate;  // save next time to poll buttons
  keyProcessed = false;
  filter_on = false;
}

void loop() {
  if (millis() >= time) {
    // it's time to carry on measurement
    // ADC measurement
    if (test) {
      ADC_read = ADC_test;
    } else {
      ADC_read = analogRead(Vin);  // samples input voltage - dummy read
      ADC_read = analogRead(Vin);  // samples input voltage - second read with stabilized input
    }
    if (test) {
      Serial.print("SF1: ");
      Serial.println(SF1);
    }
    time = time + meas_rate;  // sets next sample time
    if (ADC_0 > -1) {
      // zero compensation has been done, then resistance value may be computed
      if (ADC_read == 1023) {
        // overflow (open circuit or input resistance > 9.4 ohm)
        lcd.setCursor(0, 1);
        lcd.print("Overflow        ");
      } else {
        Rmeas_long = long(SF1) * long(ADC_read - ADC_0);  // computes with 32 bits in µohm
        Rmeas_filtered = (coeff_filt * (Rmeas_long - Rmeas_filtered) / 1000L + Rmeas_filtered);
        if (filter_on) {
          Rmeas = int((Rmeas_filtered + 500L) / 1000L);  // round to the nearest value in mohm
        } else {
          Rmeas = int((Rmeas_long + 500L) / 1000L);  // round to the nearest value in mohm
        }
        // display value on second line
        lcd.setCursor(0, 1);
        MyString = "R " + String(Rmeas) + " mohm       ";
        lcd.print(MyString);  // dispaly measured value
      }
    }
    keyProcessed = false;  // enable key processing
  }                        // end of measurement block
  // buttons management
  if (!keyProcessed && (millis() >= polling_time)) {
    // lcd_key = read_LCD_buttons();  // read the buttons
    polling_time = millis() + polling_rate;  // sets next polling time
    read_HW_buttons();
    if (calSelect)  // calibration button depressed
    {
      if (ADC_read < ADC_0_max) {
        // zero compensation validated
        if (test) {
          Serial.print("ADC_read: ");
          Serial.println(ADC_read);
        }
        ADC_0 = ADC_read;  // save offset value
        lcd.setCursor(0, 1);
        lcd.print(Blank);  // remove warning message
        if (test) { ADC_test = 960; }
      } else if ((ADC_read > ADC_min) && (ADC_read < ADC_max) && (ADC_0 > -1)) {
        // scale factor calibration validated
        SF1 = int((long(Rref) * 10000L / long(ADC_read - ADC_0) + 5L) / 10L);  // computes with 32 bits and round to the nearest value
        lcd.clear();
        MyString = "Cal " + String(SF1);
        lcd.print(MyString);       // display scale factor on 1st line
        EEPROM.put(SF1_ADD, SF1);  // save SF1 value in EEPROM
      } else {
        // SELECT button depressed with wrong input conditions
        // blink the display for warning
        for (i = 0; i <= 5; i++) {
          lcd.noDisplay();
          delay(300);
          lcd.display();
          delay(300);
        }
      }
      keyProcessed = true;  // prevents further processing of the key until next sample time
    }
    // management of mode state switch
    if (modeState) {
      if (!filter_on) {
        // a transition from OFF to ON of the state switch has been detected
        filter_on = true;             // activate filtering
        Rmeas_filtered = Rmeas_long;  // initialize filtered value with last unfiltered value upon activation of filtered mode
        current_filter = "filter on ";
      };
    } else {
      filter_on = false;  // inhibits filter
      current_filter = "filter off";
    }
    if (validateSelect) {
      // manage measurement rate
      if (meas_rate == 1000) {
        meas_rate = 100;  // switch to 100 ms measurement rate
        current_rate = "fast/";
      } else {
        meas_rate = 1000;  // switch to 1 s measurement rate
        current_rate = "slow/";
      }
      Serial.println(meas_rate);
    }
    // display current operation mode
    if (ADC_0 > -1) {
      MyString = current_rate + current_filter;
      lcd.setCursor(0, 0);
      lcd.print(MyString);
    }
    keyProcessed = true;  // prevents further processing of the key until next polling time
  }                       // end of !keyProcessed block
}
