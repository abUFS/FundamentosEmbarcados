#include <LiquidCrystal_AIP31068_I2C.h>


LiquidCrystal_AIP31068_I2C lcd(0x3E,16,2);  // set the LCD address to 0x3E for a 16 chars and 2 line display

// Define the digits array and selected array
int digits[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
int selected[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
int outOfOrder[10] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
int numEqualPosition;

const int buttonPinRight = 2;  // pin for "right" button
const int buttonPinLeft = 3;   // pin for "left" button
const int buttonPinOk = 4;     // pin for "ok" button

unsigned long last_debounce_time_right = 0;
unsigned long debounce_delay_right = 80;
bool right_state = LOW;
bool last_right_state = LOW;

unsigned long last_debounce_time_left = 0;
unsigned long debounce_delay_left = 80;
bool left_state = LOW;
bool last_left_state = LOW;

unsigned long last_debounce_time_ok = 0;
unsigned long debounce_delay_ok = 80;
bool ok_state = LOW;
bool last_ok_state = LOW;

int reading;

int counter = 0;               // counter variable
int sel1;
int sel1POS;
int sel2;
int sel2POS;
int opt = 0;
int tp;

int tempo1;
int tempo2;
int minutosJogados;
int segundosJogados;
int jogadas;

// Define the shuffleSelected function
void shuffleSelected(int arr[], int selected[], int selectedSize) {
  // initialize the random number generator
  randomSeed(analogRead(0));
  // shuffle the selected elements
  for (int i = 0; i < selectedSize; i++) {
    int j = random(i, selectedSize);
    int temp = arr[selected[i]];
    arr[selected[i]] = arr[selected[j]];
    arr[selected[j]] = temp;
  }
}

//Counts how many of the digits are in the ascending order
int countEqualPosition(int digits[]) {
  int count = 0;
  for (int i = 0; i < 10; i++) {
    if (digits[i] == i) {
      count++;
    }
  }
  return count;
}

void getIndicesDifferentFromValue(int digits[], int outOfOrder[]) {
  int j = 0;
  for (int i = 0; i < 10; i++) {
    if (digits[i] != i) {
      outOfOrder[j] = i;
      j++;
    }
  }
}

void printDigits() {
  for (int i = 0; i < sizeof(digits) / sizeof(digits[0]); i++) {
  lcd.print(digits[i]);
  //lcd.print(" ");
  }
}

/*
void printOutof(){
  lcd.setCursor(0,1);
  for (int i = 0; i < sizeof(outOfOrder) / sizeof(outOfOrder[0]); i++) {
  lcd.print(outOfOrder[i]);
  //lcd.print(" ");
  }
  lcd.print(" ");
  lcd.print(numEqualPosition);
  lcd.setCursor(0,0);  
}*/

void printSelectds(){
  lcd.clear();
  lcd.setCursor(sel1POS,1);
  lcd.print("{");
  lcd.setCursor(sel2POS,1);
  lcd.print("}");
  lcd.setCursor(0,0);
  printDigits();
}

void setup() {
  // Initialize the I2C display
  lcd.init();
  // Print a message to the display
  lcd.print("Shuffling Digits");
  shuffleSelected(digits, selected, sizeof(selected) / sizeof(selected[0]));
  delay(1000);
  // Clear the display
  lcd.clear();
  // Print the shuffled digits to the display
  printDigits();
  //printOutof();
  //printSelectds();

  pinMode(buttonPinRight, INPUT_PULLUP);  // set up "right" button pin
  pinMode(buttonPinLeft, INPUT_PULLUP);   // set up "left" button pin
  pinMode(buttonPinOk, INPUT_PULLUP);     // set up "ok" button pin
  
  Serial.begin(9600);                      // start serial communication
  tempo1 = millis();
}

void readButtons() {

  // Debounce the button
  reading = digitalRead(buttonPinRight);
  if (reading != last_right_state) {
    last_debounce_time_right = millis();
  }
  if ((millis() - last_debounce_time_right) > debounce_delay_right) {
    if (reading != right_state) {
      right_state = reading;
      if (right_state == HIGH) {
        counter++;
        if (counter > 9){counter = 0;}
        if (opt == 0){
          sel1POS = counter;
          sel1 = digits[counter];
        }
        else{
          sel2POS = counter;
          sel2 = digits[counter];
        }
        printSelectds();
        Serial.println(sel1);
        Serial.println(sel2);
        delay(100);
      }
    }
  }
  last_right_state = reading;

    // Debounce the button
  reading = digitalRead(buttonPinLeft);
  if (reading != last_left_state) {
    last_debounce_time_left = millis();
  }
  if ((millis() - last_debounce_time_left) > debounce_delay_left) {
    if (reading != left_state) {
      left_state = reading;
      if (left_state == HIGH) {
        counter--;
        if (counter < 0){counter = 9;}
        if (opt == 0){
          sel1POS = counter;
          sel1 = digits[counter];
        }
        else{
          sel2POS = counter;
          sel2 = digits[counter];
        }
        printSelectds();
        Serial.println(sel1);
        Serial.println(sel2);
        delay(100);
      }
    }
  }
  last_left_state = reading;

  // Debounce the button
  reading = digitalRead(buttonPinOk);
  if (reading != last_ok_state) {
    last_debounce_time_ok = millis();
  }
  if ((millis() - last_debounce_time_ok) > debounce_delay_ok) {
    if (reading != ok_state) {
      ok_state = reading;
      if (ok_state == HIGH) {
        opt++;
        opt = constrain(opt, 0, 2);
        for(int k = 0; k < 3; k++){
          lcd.setCursor(12,0);
          lcd.print("*");
          delay(250);
          lcd.setCursor(12,0);
          lcd.print(" ");
          }
        lcd.setCursor(0,0);
      }
    }
  }
  last_ok_state = reading;

}

void loop() {
  readButtons();
  if (opt == 2){

  tp = digits[sel1POS];
  digits[sel1POS] = digits[sel2POS];
  digits[sel2POS] = tp;

  jogadas++;

  numEqualPosition = countEqualPosition(digits);
  getIndicesDifferentFromValue(digits, outOfOrder);
  // Shuffle the selected digits every 5 seconds
  shuffleSelected(digits, outOfOrder, 10 - numEqualPosition);
  opt =0;
    // Clear the display
  lcd.clear();
  // Print the shuffled digits to the display
  printDigits();
  //printOutof();
  printSelectds();
  }
  if (numEqualPosition == 10){
    for (int k = 0; k < 5; k++){
          lcd.clear();
          lcd.print("Acertou!");
          delay(1000);
          lcd.clear();
          lcd.print("Jogadas:");
          lcd.print(jogadas);
          delay(1000);
          lcd.clear();
          lcd.print("Time:");
          lcd.print(minutosJogados);
          lcd.print("min ");
          lcd.print(segundosJogados);
          lcd.print("s");
          delay(1000);
          lcd.clear();
    }
    numEqualPosition = 0;
    lcd.print("Shuffling Digits");
    shuffleSelected(digits, selected, sizeof(selected) / sizeof(selected[0]));
    delay(1000);
    // Clear the display
    lcd.clear();
    // Print the shuffled digits to the display
    printDigits();

  }
  tempo2 = millis();
  if ((tempo2 - tempo1) >= 1000){
    segundosJogados++;
    if (segundosJogados == 60){
      minutosJogados++;
      segundosJogados = 0;
    }
    
    tempo1 = tempo2;
  }


  // Wait 5 seconds before shuffling again

  

}