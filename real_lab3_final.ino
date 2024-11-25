// EECEE 5250 Lab 3
// Winflash Hwang
// 11/24/2024


#include <avr/io.h>
#include <avr/interrupt.h>
#include "IRremote.hpp"
#include <Wire.h>
#include <LiquidCrystal.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include "arduinoFFT.h"

#define SAMPLES 256               // Number of samples for better resolution
#define SAMPLING_FREQUENCY 8192   // Adjust sampling frequency for higher resolution
#define MIC_PIN A0                // Microphone input pin

ArduinoFFT<double> FFT = ArduinoFFT<double>();  // Instantiate the FFT object with double data type
unsigned int samplingPeriod;
unsigned long previousMillis = 0;  // Stores last time frequency was printed
const long interval = 1000;  // Interval at which to print frequency (1000 ms = 1 second)

double vReal[SAMPLES]; // Create vector of size SAMPLES to hold real values
double vImag[SAMPLES]; // Create vector of size SAMPLES to hold imaginary values

volatile int flag100ms = 0; // Flag for 100ms intervals
volatile int flag500ms = 0; // Flag for 500ms intervals
volatile int toggle3 = 0; // Use int for toggle
volatile int DIRAstate = HIGH;
volatile int DIRBstate = LOW;

#define IR_RECEIVE_PIN 7  // The Arduino pin connected to IR receiver
const int rs = 27, en = 28, d4 = 25, d5 = 24, d6 = 23, d7 = 22;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define ENABLE 3  // PWM-capable pin
#define DIRA 5
#define DIRB 4
#define buttonPin 13 // pin of the switch button 

int buttonState = 0;
//int speed = 100;  // PWM duty cycle value (0-255)
int speed = 0;
const char* speedValues[] = {"0", "1/2", "3/4", "Full"};
const char* speedValue = "";
String spin;
int n;

IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

bool updateMode = false;
String inputTime = "";
bool isTimeRunning = true;

void setup() {

  cli(); // Stop interrupts

  // Configure Timer2 for 100ms interrupts
  TCCR2A = 0; // Clear TCCR2A
  TCCR2B = 0; // Clear TCCR2B
  TCNT2 = 0; // Initialize counter value to 0
  OCR2A = 155; // Set compare match register for 100ms intervals (16MHz / (1024 * 100Hz) - 1)
  TCCR2A |= (1 << WGM21); // CTC mode
  TCCR2B |= (1 << CS22) | (1 << CS21) | (1 << CS20); // 1024 prescaler
  TIMSK2 |= (1 << OCIE2A); // Enable Timer2 compare interrupt

  // Configure Timer1 for 500ms interrupts
  TCCR1A = 0; // Clear TCCR1A
  TCCR1B = 0; // Clear TCCR1B
  TCNT1  = 0; // Initialize counter value to 0
  OCR1A = 31249; // Set compare match register for 500ms intervals (16MHz / (256 * 2Hz) - 1)
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS12); // 256 prescaler
  TIMSK1 |= (1 << OCIE1A); // Enable Timer1 compare interrupt

  sei(); // Enable global interrupts

  Serial.begin(115200);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  while (!Serial) ;  // wait for serial
  pinMode(ENABLE, OUTPUT);  // Set ENABLE pin as output
  pinMode(DIRA, OUTPUT);    // Set DIRA pin as output
  pinMode(DIRB, OUTPUT);    // Set DIRB pin as output
  pinMode(buttonPin, INPUT);
  lcd.begin(16, 2); 
  Serial.println("DS1307RTC Read Test");
  Serial.println("-------------------");
  setRTCTime(12, 30, 0);  // Initial time setup
  lcd.setCursor(0, 1);
  spin = "C";
  n = 0;
  lcd.print(spin);
  pinMode(MIC_PIN, INPUT);
  samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY)); // Period in microseconds
}

ISR(TIMER2_COMPA_vect) {
  flag100ms = 1; // Set the flag for 100ms interval
}

ISR(TIMER1_COMPA_vect) {
  flag500ms = 1; // Set the flag for 500ms interval
}

void loop() {
  if (IrReceiver.decode()) { 
    uint16_t command = IrReceiver.decodedIRData.command; 
    Serial.print("Received command: 0x"); 
    Serial.println(command, HEX);
    
    if (updateMode) {
      switch (command) {
        case 0x16:  // 0
          inputTime += "0";
          break;
        case 0x0C:  // 1
          inputTime += "1";
          break;
        case 0x18:  // 2
          inputTime += "2";
          break;
        case 0x5E:  // 3
          inputTime += "3";
          break;
        case 0x08:  // 4
          inputTime += "4";
          break;
        case 0x1C:  // 5
          inputTime += "5";
          break;
        case 0x5A:  // 6
          inputTime += "6";
          break;
        case 0x42:  // 7
          inputTime += "7";
          break;
        case 0x52:  // 8
          inputTime += "8";
          break;
        case 0x4A:  // 9
          inputTime += "9";
          break;
        case 0x0:  // Handle unexpected zero input gracefully
          Serial.println("Received unexpected zero command.");
          break;
        default:
          Serial.println("Exiting update mode due to non-numeric input.");
          updateMode = false;  // Exit update mode on any non-numeric input
          isTimeRunning = true;  // Resume time
          inputTime = "";  // Reset inputTime
          return;  // Exit the loop early
      }

      Serial.print("Current inputTime: ");
      Serial.println(inputTime);  // Debugging: print current inputTime
      if (inputTime.length() == 6) {
        // Once 6 digits are entered, update the RTC time
        int setHour = inputTime.substring(0, 2).toInt();
        int setMinute = inputTime.substring(2, 4).toInt();
        int setSecond = inputTime.substring(4, 6).toInt();
        setRTCTime(setHour, setMinute, setSecond);
        updateMode = false;  // Exit update mode
        isTimeRunning = true;  // Resume time
        inputTime = "";  // Reset inputTime
      }
    } else if (command == 0x46) {  // FUNC/STOP button
      Serial.println("Entering update mode...");
      updateMode = true;  // Enter update mode
      isTimeRunning = false;  // Stop time
    } else {
      // Handle other button presses
      switch (command) {
        case 0x45:  // Power button
          Serial.println("Power");
          break;
        case 0x46:  // FUNC/STOP button
          Serial.println("FUNC/STOP");
          break;
        case 0x47:  // CH+
          Serial.println("CH+");
          break;
        case 0x44:  // <<
          Serial.println("<<");
          break;
        case 0x40:  // >>
          Serial.println(">>");
          break;
        case 0x43:  // PLAY/PAUSE
          Serial.println(">||");
          break;
        case 0x07:  // VOL-
          Serial.println("-");
          break;
        case 0x15:  // VOL+
          Serial.println("+");
          break;
        case 0x09:  // EQ
          Serial.println("EQ");
          break;
        case 0x19:  // 100+
          Serial.println("100+");
          break;
        case 0x0D:  // 200+
          Serial.println("200+");
          break;
        case 0x16:  // 0
          Serial.println("0");
          break;
        case 0x0C:  // 1
          Serial.println("1");
          break;
        case 0x18:  // 2
          Serial.println("2");
          break;
        case 0x5E:  // 3
          Serial.println("3");
          break;
        case 0x08:  // 4
          Serial.println("4");
          break;
        case 0x1C:  // 5
          Serial.println("5");
          break;
        case 0x5A:  // 6
          Serial.println("6");
          break;
        case 0x42:  // 7
          Serial.println("7");
          break;
        case 0x52:  // 8
          Serial.println("8");
          break;
        case 0x4A:  // 9
          Serial.println("9");
          break;
        default:
          Serial.println("WARNING: undefined command:");
          break;
      }
    }
    IrReceiver.resume();
  }

  if (isTimeRunning) {
    displayTime();
  }
  //This two 100ms time interrupts work for preventing rapid changes 
  if (flag100ms == 1) { // Check if the 100ms flag is set
    flag100ms = 0; // Clear the flag
    TI();
  }
  if (flag100ms == 1) { // Check if the 100ms flag is set
    flag100ms = 0; // Clear the flag
    TI();
  }

for (int i = 0; i < SAMPLES; i++) {
    unsigned long microSeconds = micros(); // Returns the number of microseconds since the Arduino board began running the current script.  
    
    vReal[i] = analogRead(MIC_PIN); // Reads the value from analog pin 0 (A0), quantize it and save it as a real term.
    Serial.print(vReal[i]); // Print raw ADC values for debugging 
    Serial.print(" ");
     
    vImag[i] = 0; // Makes imaginary term 0 always

    // Remaining wait time between samples if necessary
    while (micros() < (microSeconds + samplingPeriod)) {
      // Do nothing
    }
  }
  Serial.println(); // New line after printing ADC values

  // Perform FFT on samples
  FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.complexToMagnitude(vReal, vImag, SAMPLES);

  double peakFrequency = FFT.majorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);

  // Get the current time in milliseconds
  // Check if the current time is greater than the previous time by the interval
  
    
    // Print the detected frequency
    Serial.println(vReal[50]);
    Serial.println("\n");
    

  if (flag100ms == 1) { // Check if the 100ms flag is set
    flag100ms = 0; // Clear the flag
    TI();
  }

  if(vReal[50]>7.00 && vReal[50]<15.00){
     n=n-1;
     if(n<=-1){n++;}
  }
  else if(vReal[50]>25.00){
      n=n+1;
      if(n>4){n--;}
  }

  if (strcmp(speedValues[n], "0") == 0) {
    speed = 0; 
    speedValue = "0"; 
  } 
  else if (strcmp(speedValues[n], "1/2") == 0) {
    speed = 90;  
    speedValue = "1/2";
  } 
  else if (strcmp(speedValues[n], "3/4") == 0) {
    speed = 180;  
    speedValue = "3/4";
  } 
  else if (strcmp(speedValues[n], "Full") == 0) {
    speed = 255; 
    speedValue = "Full"; 
  }    

  if (flag500ms == 1) { // Check if the 500ms flag is set
    flag500ms = 0; // Clear the flag
    
    readButtonState();
    lcd.clear();
  }

  lcd.setCursor(0, 1);
  lcd.print(spin);
  lcd.setCursor(5, 1);
  lcd.print(speedValue);
  // Always control motor based on state
  digitalWrite(DIRA, DIRAstate);  // Set motor direction
  digitalWrite(DIRB, DIRBstate);
  analogWrite(ENABLE, speed); 

}

void TI() {
  if (toggle3 == 1) {
    toggle3 = 0;
  } else {
    toggle3 = 1;
  }
}

void readButtonState() {
  buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {
    if (DIRAstate == HIGH && DIRBstate == LOW) {
      
      DIRAstate = LOW;
      DIRBstate = HIGH;
      analogWrite(ENABLE, 0);
      lcd.setCursor(0, 1);
      spin = "CC";
      Serial.println("Counter Clockwise rotation");
    } else if (DIRAstate == LOW && DIRBstate == HIGH) {
      
      DIRAstate = HIGH;
      DIRBstate = LOW;
      analogWrite(ENABLE, 0);
      lcd.setCursor(0, 1);
      spin = "C";
      Serial.println("Clockwise rotation");
    }
  }
}

void displayTime() {
  tmElements_t tm;
  RTC.read(tm);

  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  print2digits(tm.Hour);
  lcd.print(":");
  print2digits(tm.Minute);
  lcd.print(":");
  print2digits(tm.Second);

  Serial.print("Time: ");
  Serial.print(tm.Hour);
  Serial.print(":");
  Serial.print(tm.Minute);
  Serial.print(":");
  Serial.println(tm.Second);
}

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    lcd.print('0');
    Serial.print('0');
  }
  lcd.print(number);
  Serial.print(number);
}

// Function to set the RTC time
void setRTCTime(int hour, int minute, int second) {
  tmElements_t tm;
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = second;
  tm.Day = 24;  // You can set the day, month, and year here if needed
  tm.Month = 11;
  tm.Year = CalendarYrToTm(2024);

  if (RTC.write(tm)) {
    Serial.println("RTC time set successfully!");
  } else {
    Serial.println("RTC write error!");
  }
}


