// Define pin connections
int latchPin = 28;  // RCLK
int clockPin = 22; // SRCLK
int dataPin = 23;  // SER
int srclrPin = 24; // SRCLR, connect to Vcc if not controlled by code

const int digitPins[2] = {12, 11}; // Pins for the 2 digits
const int segmentPins[10] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00100111, 0b01111111, 0b01101111};
int countdown = 20; // countdown time in seconds

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(srclrPin, OUTPUT);
  digitalWrite(srclrPin, HIGH); // Ensure shift register is not cleared
  for (int i = 0; i < 2; i++) {
    pinMode(digitPins[i], OUTPUT);
    digitalWrite(digitPins[i], LOW); // Turn off all digits initially
  }
}

void loop() {
  for (int i = countdown; i >= 0; i--) {
     // Display each number 200 times for a total duration of 1 second
     for(int j = 0; j<100; j++){
       
       displayNumber(i);
       }
      // Wait for 1 second
  }
  clearDisplay(); // Clear display
  delay(20000); // Wait for 20 seconds before restarting
}

void displayNumber(int num) {
  int digits[2];
  digits[0] = num / 10;
  digits[1] = num % 10;
  
  for (int i = 0; i < 2; i++) {
    digitalWrite(digitPins[i], HIGH);
    shiftOut(dataPin, clockPin, MSBFIRST, segmentPins[digits[i]]);
    digitalWrite(latchPin, HIGH);
    digitalWrite(latchPin, LOW);
    delay(5); // Short delay to display each digit
    digitalWrite(digitPins[i], LOW);
    
  }
}

void clearDisplay() {
  shiftOut(dataPin, clockPin, MSBFIRST, 0x00);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}



