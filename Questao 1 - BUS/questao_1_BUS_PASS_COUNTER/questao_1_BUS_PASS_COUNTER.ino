#include <Wire.h>
#include <LiquidCrystal_AIP31068_I2C.h>
#include <avr/sleep.h>
#include <EEPROM.h>

const int EEPROM_SIZE = 512;
const int COUNTER_ADDR = 0; // address to store the counter
const int ARRAY_ADDR = 2; // address to store the array

LiquidCrystal_AIP31068_I2C lcd(0x3E,16,2);  // set the LCD address to 0x3E for a 16 chars and 2 line display

const int BUTTON_PIN = 2;
const int RESET_PIN = 7;
const int SLEEP_PIN = 4;

int counter = 0;
int last_counters[7] = {0, 0, 0, 0, 0, 0, 0};


bool system_reset = false;

unsigned long last_debounce_time_button = 0;
unsigned long debounce_delay_button = 50;
bool button_state = LOW;
bool last_button_state = LOW;

unsigned long last_debounce_time_reset = 0;
unsigned long debounce_delay_reset = 50;
bool reset_state = LOW;
bool last_reset_state = LOW;

unsigned long last_debounce_time_sleep = 0;
unsigned long debounce_delay_sleep = 50;
bool sleep_state = LOW;
bool last_sleep_state = LOW;

int reading = digitalRead(BUTTON_PIN);

void setup() {
  lcd.init();

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, INPUT_PULLUP);
  pinMode(SLEEP_PIN, INPUT_PULLUP);

  //EEPROM.begin(EEPROM_SIZE);
  //EEPROM.put(ARRAY_ADDR, last_counters);
  //EEPROM.write(COUNTER_ADDR, counter);
  //EEPROM.commit();
  //counter = EEPROM.read(COUNTER_ADDR);
  //EEPROM.get(ARRAY_ADDR, last_counters);
}

/*
void copyA(int* src, int* dst, int len) {
    memcpy(dst, src, sizeof(src[0])*len);
}*/

void wakeUpNow() {
  //
}

void goToSleep() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(0, wakeUpNow, LOW);
  sleep_mode();
  sleep_disable();         // first thing after waking from sleep:
  detachInterrupt(0);
}

void loop() {
  goToSleep();
  while (digitalRead(SLEEP_PIN)) {
    lcd.setCursor(0, 0);
    lcd.print("Counter: ");
    lcd.print(counter);

    // Debounce the button
    reading = digitalRead(BUTTON_PIN);
    if (reading != last_button_state) {
      last_debounce_time_button = millis();
    }
    if ((millis() - last_debounce_time_button) > debounce_delay_button) {
      if (reading != button_state) {
        button_state = reading;
        if (button_state == HIGH) {
          counter++;
          EEPROM.write(COUNTER_ADDR, counter);
        }
      }
    }
    last_button_state = reading;

    // Debounce the button
    reading = digitalRead(SLEEP_PIN);
    if (reading != last_sleep_state) {
      last_debounce_time_sleep = millis();
    }
    if ((millis() - last_debounce_time_sleep) > debounce_delay_sleep) {
      if (reading != sleep_state) {
        sleep_state = reading;
        if (sleep_state == HIGH) {
          sleep_mode(); // put the Arduino into sleep mode
        }
      }
    }
    last_sleep_state = reading;

    // Debounce the reset button
    reading = digitalRead(RESET_PIN);
    if (reading != last_reset_state) {
      last_debounce_time_reset = millis();
    }
    if ((millis() - last_debounce_time_reset) > debounce_delay_reset) {
      if (reading != reset_state) {
        reset_state = reading;
        if (reset_state == HIGH) {
          lcd.clear();
          lcd.print("Resetting...");
          delay(1000);

          last_counters[6] = last_counters[5];
          last_counters[5] = last_counters[4];
          last_counters[4] = last_counters[3];
          last_counters[3] = last_counters[2];
          last_counters[2] = last_counters[1];
          last_counters[1] = last_counters[0];
          last_counters[0] = counter;

          EEPROM.put(ARRAY_ADDR, last_counters);
          EEPROM.write(COUNTER_ADDR, counter);

          counter = 0;
          system_reset = true;
        }
      }
    }
    last_reset_state = reading;

    if (system_reset) {
      lcd.clear();
      lcd.print("Diff: ");
      lcd.print(last_counters[0] - last_counters[1]);
      delay(2000);
      lcd.clear();
      system_reset = false;
      }
  }

  
}