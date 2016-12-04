#include <Arduino.h>
#include <SimpleTimer.h>
#include <LiquidCrystal.h>
#include <RTC.h>
#include "Logging.h"

////////////////////////////////////////////////////////////////////////////////
// Pin configuration

const byte SOLAR_CURRENT_PIN = A0;
const byte CHARGER_CURRENT_PIN = A1;
const byte SOLAR_VOLTAGE_PIN = A2;
const byte BATTERY_VOLTAGE_PIN = A3;

const byte LCD_RS_PIN = 4;
const byte LCD_ENABLE_PIN = 5;
const byte LCD_D0_PIN = 6;
const byte LCD_D1_PIN = 7;
const byte LCD_D2_PIN = 8;
const byte LCD_D3_PIN = 9;

const byte CHARGE_SELECTOR_PIN = 10;
const byte MASTER_POWER_ENABLE_PIN = 11;

////////////////////////////////////////////////////////////////////////////////
// Constants

const float LOW_VOLTAGE_CUTOFF = 12.0;
const int CURRENT_SCALAR = 185;
const float SOLAR_VOLTAGE_SCALAR = 4.0;
const float BATTERY_VOLTAGE_SCALAR = 2.0;
const int DATETIME_BUFFER_WIDTH = 16;

const long TIME_UPDATE_PERIOD = 1000;
const long SOLAR_UPDATE_PERIOD = 1000;
const long CHARGER_UPDATE_PERIOD = 1000;
const long BATTERY_UPDATE_PERIOD = 1000;

enum CHARGERS{
    SOLAR = HIGH,
    MAINS = LOW
};

enum POWER_STATES{
    ON = HIGH,
    OFF = LOW
};

enum DISPLAY_PAGES{
    DISP_SOLAR = 0,
    DISP_BATTERY = 1
};

////////////////////////////////////////////////////////////////////////////////
// Variables

int solar_current;
float solar_voltage;
int charger_current;
float battery_voltage;
char datetime[DATETIME_BUFFER_WIDTH];

bool charger_supply = MAINS;
bool master_power_state = OFF;

LiquidCrystal display(LCD_RS_PIN, LCD_ENABLE_PIN, LCD_D0_PIN, LCD_D1_PIN, LCD_D2_PIN, LCD_D3_PIN);
RTC_DS3231 rtc;
SimpleTimer timer;

////////////////////////////////////////////////////////////////////////////////
// Main Functions

void setup(){
    Log.Init(LOG_LEVEL_DEBUG, 57600);
    Log.Info("Security system power monitor");

    initialise_pins();
    start_rtc();
    display.begin(16, 2);
}

void loop(){
    timer.run();
}

////////////////////////////////////////////////////////////////////////////////
// Functions

void initialise_pins(){
    pinMode(SOLAR_CURRENT_PIN, INPUT);
    pinMode(CHARGER_CURRENT_PIN, INPUT);
    pinMode(SOLAR_VOLTAGE_PIN, INPUT);
    pinMode(BATTERY_VOLTAGE_PIN, INPUT);
    pinMode(LCD_RS_PIN, OUTPUT);
    pinMode(LCD_ENABLE_PIN, OUTPUT);
    pinMode(LCD_D0_PIN, OUTPUT);
    pinMode(LCD_D1_PIN, OUTPUT);
    pinMode(LCD_D2_PIN, OUTPUT);
    pinMode(LCD_D3_PIN, OUTPUT);
    pinMode(CHARGE_SELECTOR_PIN, OUTPUT);
    pinMode(MASTER_POWER_ENABLE_PIN, OUTPUT);
}

int get_solar_current(){
    /**
    * Get the current from the solar charger
    * @return Charge current in mA
    */
    int raw_current = analogRead(SOLAR_CURRENT_PIN);
    return raw_current * CURRENT_SCALAR;
}

float get_solar_voltage(){
    /**
    * Get the voltage from the solar charger
    * @return Voltage in volts
    */
    int raw_voltage = analogRead(SOLAR_VOLTAGE_PIN);
    return float(raw_voltage) * SOLAR_VOLTAGE_SCALAR;
}

int get_charger_current(){
    /**
    * Get the current from the mains charger
    * @return Charge current in mA
    */
    int raw_current = analogRead(CHARGER_CURRENT_PIN);
    return raw_current * CURRENT_SCALAR;
}

float get_battery_voltage(){
    /**
    * Get the voltage of the battery
    * @return Battery voltage in volts
    */
    int raw_voltage = analogRead(BATTERY_VOLTAGE_PIN);
    return float(raw_voltage) * BATTERY_VOLTAGE_SCALAR;
}

void check_charger_supply(){
    // TODO - Charger switching logic
}

void switch_charger(int charger){
    /**
    * Switch the battery charger to the specified supply.
    * @param charger Charger ID of the supply {SOLAR = 1, MAINS = 0}
    */
    if (charger == SOLAR){
        digitalWrite(CHARGE_SELECTOR_PIN, SOLAR);
    }
    else{
        charger = MAINS;
        digitalWrite(CHARGE_SELECTOR_PIN, MAINS);
    }
    charger_supply = charger;
}

void check_battery(){
    // TODO  - Master switching logic
}

void switch_master_power(bool state){
    /**
    * Switch the master power to the security system to the specified state
    * @param state Power state of the security system {ON = 1, OFF = 0}
    */
    if (state == ON){
        digitalWrite(MASTER_POWER_ENABLE_PIN, ON);
    }
    else{
        state = OFF;
        digitalWrite(MASTER_POWER_ENABLE_PIN, OFF);
    }
    master_power_state = state;
}

////////////////////////////////////////////////////////////////////////////////
// RTC

void start_rtc() {
  /**
  * Start up the real-time clock
  */
  rtc.begin();
  timer.setInterval(1000, update_timestamp);
}

void update_timestamp() {
  /**
  * Read the timestamp into the current entry
  */

  get_datetime(datetime);
  Log.Debug(P("Time: %s"), datetime);
}

long get_timestamp() {
  /**
  * Get the current timestamp.
  * @return Long timestamp format (seconds since 2000-01-01)
  */
  DateTime now = rtc.now();
  return now.secondstime();
}

void get_datetime(char *buffer) {
  /** Get the datetime in string format
  * DateTime follows the standard ISO format ("YYYY-MM-DD hh:mm:ss")
  * @param buffer Character buffer to write datetime to; Must be at least 20
  * char wide.
  */
  DateTime now = rtc.now();

  // Get the DateTime into the standard, readable format
  sprintf(buffer, P("%04d-%02d-%02d %02d:%02d"), now.year, now.month(),
          now.day(), now.hour(), now.minute(), now.second());
}

////////////////////////////////////////////////////////////////////////////////
// Display
// TODO - Display pages

void update_display(){
    return;
}
