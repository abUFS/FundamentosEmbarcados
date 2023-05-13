#include <Wire.h>
#include <LiquidCrystal_AIP31068_I2C.h>
#include <EEPROM.h>
#include <stdlib.h>

struct Player {
  int score;
  char nick[6];
};


Player players[5];

const int GREEN_LED = 3;
const int GREEN_BUTTON = 6;

const int RED_LED = 5;
const int RED_BUTTON = 9;

const int YELLOW_LED = 2;
const int YELLOW_BUTTON = 7;

const int BLUE_LED = 4;
const int BLUE_BUTTON = 8;

const int LEDS[] = {GREEN_LED, RED_LED, YELLOW_LED, BLUE_LED};
const int NUM_LEDS = sizeof(LEDS) / sizeof(LEDS[0]);

const int BUTTONS[] = {GREEN_BUTTON, RED_BUTTON, YELLOW_BUTTON, BLUE_BUTTON};
const int NUM_BUTTONS = sizeof(BUTTONS) / sizeof(BUTTONS[0]);

LiquidCrystal_AIP31068_I2C lcd(0x3E,16,2);  // set the LCD address to 0x3E for a 16 chars and 2 line display

int score = 0;

int sequence[256];

void saveScore(const char* nick, int score) {
    int i;
    for (i = 0; i < 5; i++) {
        if (players[i].score <= score) {
            break;
        }
    }
    for (int j = 5 - 1; j > i; j--) {
            players[j] = players[j - 1];
            EEPROM.put(j*sizeof(Player), players[j]);
            Serial.print("Moved to: ");
            Serial.println(j*sizeof(Player));
        }
    strcpy(players[i].nick, nick);
    players[i].score = score;
    EEPROM.put(i*sizeof(Player), players[i]);
    Serial.print("Saved in :");
    Serial.println(i*sizeof(Player));
}

void initPlayersEEPROM(){
  for (int i = 0; i < 5; i++) {
    EEPROM.get(sizeof(Player) * i, players[i]);
    Serial.print(players[i].nick);
    Serial.print(", ");
    Serial.println(players[i].score);
  }
}

void printPlayerFromEEPROM() {
  for (int i = 0; i < 5; i++) {
    if (players[i].nick[0] != NULL){
        Serial.print(players[i].nick);
        Serial.print(", ");
        Serial.println(players[i].score);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(i);
        lcd.print(" ");
        lcd.print(players[i].nick);
        lcd.print(" ");
        lcd.print(players[i].score);
        if (i < 4){
          lcd.setCursor(0, 1);
          lcd.print(i + 1);
          lcd.print(" ");
          lcd.print(players[i + 1].nick);
          lcd.print(" ");
          lcd.print(players[i + 1].score);
        }
        delay(1500);
        lcd.setCursor(0, 0);
        lcd.clear();
    }

  }
}

int getRandomLed() {
  return LEDS[random(NUM_LEDS)];
}

void blinkLed(int ledPin, int multi) {
  digitalWrite(ledPin, HIGH);
  delay(multi*100);
  digitalWrite(ledPin, LOW);
  delay(multi*100);
}

int readButtonsAndBlinkLed(int multi = 5) {
  const int DEBOUNCE_TIME = 50;
  static unsigned long lastButtonTime[NUM_BUTTONS] = {0};
  static bool lastButtonState[NUM_BUTTONS] = {0};
  
  for (int i = 0; i < NUM_BUTTONS; i++) {
    bool buttonState = digitalRead(BUTTONS[i]);
    if (buttonState != lastButtonState[i]) {
      lastButtonTime[i] = millis();
    }
    
    if (millis() - lastButtonTime[i] > DEBOUNCE_TIME) {
      if (buttonState == HIGH) {
        blinkLed(LEDS[i], multi);
        lastButtonState[i] = buttonState;
        return LEDS[i];
      }
    }
    
    lastButtonState[i] = buttonState;
  }
  return -1;
}

bool getAnswer(){
  //printScore();
  lcd.setCursor(0, 1);
  lcd.print("Enter LEDs seq.");
  int i = 0;
  int pressed;
  while(i < score + 1){
    pressed = readButtonsAndBlinkLed(5);
    if(pressed >= 0){
      if (pressed == sequence[i]){
        i++;
      }
      else{
        return false;
      }

    }

  }
  return true;
}

void printScore(){
  lcd.setCursor(0, 0);
  lcd.print("Score: ");
  lcd.print(score);
}

char selected_letters[] = {95,95,95,95,95, '\0'};
void selectName(){
  lcd.setCursor(0, 0);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter your nick:");
  lcd.setCursor(0, 1);
  lcd.print(selected_letters);
  int i = 0;
  int j = 64;
  int pressed;
  while(i < 5){
    lcd.setCursor(i, 1);
    lcd.blink();
    pressed = readButtonsAndBlinkLed(2);
    if(pressed >= 0){
      if (pressed == GREEN_LED){
        if(j == 95){j = 64;}
        j++;
        if (j > 90){j = 95;}
        selected_letters[i] = j;
        lcd.setCursor(0, 1);
        lcd.print(selected_letters);
      }
      if (pressed == BLUE_LED){
        if(j == 95){j = 91;}
        j--;
        if (j < 65){j = 95;}
        selected_letters[i] = j;
        lcd.setCursor(0, 1);
        lcd.print(selected_letters);
      }
      if (pressed == YELLOW_LED){
        i++;
        j = selected_letters[i];
      }
      if (pressed == RED_LED){
        i--;
        j = selected_letters[i];
      }

    }
  }
  for (i = 0; i < 5; i++){
    if(selected_letters[i] == 95){selected_letters[i] = 32;}
  }
}

void enterNameInHighScores(){
  selectName();
  Serial.println("Saving highscore");
  char name[6];
  strcpy(name, selected_letters);
  //itoa(selected_letters, name, 10);
  saveScore(name, score);
  strcpy(selected_letters, "_____");
}

void startProcedures(){
  score = 0;
  lcd.setCursor(0, 0);
  lcd.print("New Game!");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Start!");
  delay(1500);
  lcd.clear();
  printScore();
}

void loseProcedures(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Wrong sequence!");
  delay(1500);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Final Score: ");
  lcd.print(score);
  delay(1500);

  enterNameInHighScores();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Highscores :::");
  delay(1500);
  printPlayerFromEEPROM();
  startProcedures();

}

void setup() {
  initPlayersEEPROM();
  lcd.init();
  Serial.begin(9600);
  randomSeed(analogRead(A0));
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LEDS[i], OUTPUT);
  }
  startProcedures();
}

void loop() {
  //randomSeed();
  sequence[score] = getRandomLed();
  Serial.print("Random LED: ");
  Serial.println(sequence[score]);
  for (int i = 0; i < score + 1; i++) {
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Pay attention!");
    delay(500);
    blinkLed(sequence[i], 5);
    lcd.clear();
    printScore();
  }
  delay(1000);
  if (getAnswer()){
    score++;
    printScore();
  }
  else{
    Serial.println("Perdeu");
    loseProcedures();
  }
  
}
