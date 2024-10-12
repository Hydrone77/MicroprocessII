#include <Keypad.h>

int latchPin = 28; // RCLK
int clockPin = 22; // SRCLK
int dataPin =  23; // SER
int srclrPin = 24; // SRCLR, connect to Vcc if not controlled by code
const int digitPins[2] = {50, 51};
const int segmentPins[10] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00100111, 0b01111111, 0b01101111};

int LED_G = 36;
int LED_Y = 37;
int LED_R = 39;
int LED_G2 = 10;
int LED_Y2 = 11;
int LED_R2 = 12;
int Buzz_pin = 42;

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {

 {'1','2','3','A'},
 {'4','5','6','B'},
 {'7','8','9','C'},
 {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS,COLS);

char input_key[3];// this is the array to save the letter and numbers from the keypad
int n = 0; 

volatile int flag1 = 0; // Use int for flag
volatile int flag2 = 0; // Use int for flag
volatile int flag3 = 0; // Use int for flag
volatile int toggle1 = 0; // Use int for toggle
volatile int toggle2 = 0; // Use int for toggle
volatile int toggle3 = 0; // Use int for toggle

void setup(){
 Serial.begin(9600);
 pinMode(LED_G, OUTPUT);
 pinMode(LED_Y, OUTPUT);
 pinMode(LED_R, OUTPUT);
 pinMode(LED_G2, OUTPUT);
 pinMode(LED_Y2, OUTPUT);
 pinMode(LED_R2, OUTPUT);
 pinMode(Buzz_pin, OUTPUT);
 pinMode(latchPin, OUTPUT);
 pinMode(clockPin, OUTPUT);
 pinMode(dataPin, OUTPUT);
 pinMode(srclrPin, OUTPUT);
 digitalWrite(srclrPin, HIGH); // Ensure shift register is not cleared
 for (int i = 0; i < 2; i++) {
   pinMode(digitPins[0], OUTPUT);
   pinMode(digitPins[1], OUTPUT);
   digitalWrite(digitPins[i], HIGH); // Turn off all digits initially
   }
  // Right below is time interrupt.
   cli(); // Stop interrupts

  // Set Timer1 interrupt at 1Hz (every 1 second)
  TCCR1A = 0; // Set entire TCCR1A register to 0
  TCCR1B = 0; // Same for TCCR1B
  TCNT1 = 0; // Initialize counter value to 0
  OCR1A = 15624; // = (16*10^6) / (1*1024) - 1
  TCCR1B |= (1 << WGM12); // Turn on CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10); // Set CS12 and CS10 bits for 1024 prescaler
  TIMSK1 |= (1 << OCIE1A); // Enable timer compare interrupt

  // Set Timer3 interrupt at 5Hz (every 0.2 seconds)
  TCCR3A = 0; // Set entire TCCR3A register to 0
  TCCR3B = 0; // Same for TCCR3B
  TCNT3 = 0; // Initialize counter value to 0
  OCR3A = 2499; // = (16*10^6) / (128*5) - 1
  TCCR3B |= (1 << WGM32); // Turn on CTC mode
  TCCR3B |= (1 << CS32) | (1 << CS30); // Set CS32 and CS30 bits for 128 prescaler
  TIMSK3 |= (1 << OCIE3A); // Enable timer compare interrupt

  // Set Timer2 interrupt at 200Hz (every 5 milliseconds)
  TCCR2A = 0; // Set entire TCCR2A register to 0
  TCCR2B = 0; // Same for TCCR2B
  TCNT2 = 0; // Initialize counter value to 0
  OCR2A = 125; // = (16*10^6) / (64 * 200) - 1
  TCCR2A |= (1 << WGM21); // Turn on CTC mode
  TCCR2B |= (1 << CS22) | (1 << CS20); // Set CS22 and CS20 bits for 128 prescaler
  TIMSK2 |= (1 << OCIE2A); // Enable timer compare interrupt

  sei(); // Allow interrupts

}

ISR(TIMER1_COMPA_vect) {
  flag1 = 1; // Set the flag for the interval
}

ISR(TIMER3_COMPA_vect) {
  flag2 = 1; // Set the flag for the interval
}

ISR(TIMER2_COMPA_vect) {
  flag3 = 1; // Set the flag for the interval
}
int t_R=0;// time for red LED to turn on.
int t_G=0;// time for green LED to turn on.

void loop(){
   //if loop to start flashing red LED.
   if (flag1 == 1) { // Check if the flag is set
    flag1 = 0; // Clear the flag
    if (toggle1 == 1) {
      digitalWrite(LED_R, HIGH);
      toggle1 = 0;
    } else {
      digitalWrite(LED_R, LOW);
      toggle1 = 1;
    }
  }
  
 char customKey = customKeypad.getKey();
 
 if(customKey){
   
   if(customKey =='#'){//time duration test for red and green LED.
     
     if(input_key[0] == 'A'){// A is for red LED.
      //since second and third array char value is for duration, it needs to be changed as "int" to calculate the time to turn on.
      int intValue1 = int(input_key[1] - '0');
      int intValue2 = int(input_key[2] - '0');
      // checking if each intValue is a singe integral the letters won't get in because it is bigger than 10 as "ASCII" code decimal. 
     if(intValue1 >=0 && intValue1 < 10 && intValue2>=0 && intValue2 < 10){
      
       t_R = intValue1*10 + intValue2;//second array number is 10's digit, and third array number is 1's digit.
      
     }
     }
     else if(input_key[0] == 'B'){// B is for green LED.
       int intValue1 = int(input_key[1] - '0');
       int intValue2 = int(input_key[2] - '0'); 
       // checking if each intValue is a singe integral the letters won't get in because it is bigger than 10 as "ASCII" code decimal.
       if(intValue1 >=0 && intValue1 < 10 && intValue2 >=0 && intValue2 < 10){ 
       
       t_G = intValue1*10 + intValue2;//second array number is 10's digit, and third array number is 1's digit.

       }
     else{Serial.print("Invalid Input!");}// if the input of keypad is not valid or if it is more than a 10 as "ASCII" value.
    for(int i=0; i<3; i++){
     input_key[i] ="";//clear array to put new code
      n=0;
     }    
   }
   }

   else if(customKey =='*'){//This is for RGY signal lights
     while(true){
      for (int i = t_R; i >= 4; i--) { 
        for(int j = 0; j<100; j++){//this loop holds displayNumber for 1 second
         digitalWrite(LED_R, HIGH);
         digitalWrite(LED_G2, HIGH);
         displayNumber(i);
         }
       }
         
     for (int i = 3; i >= 0; i--) { 
       digitalWrite(Buzz_pin, HIGH);
       for(int j = 0; j<50; j++){//this loop holds displaynumber and LED for 0.5 second
         digitalWrite(LED_R, HIGH);
         digitalWrite(LED_G2, HIGH);
         displayNumber(i);
         }
       for(int j = 0; j<50; j++){
         digitalWrite(LED_R, LOW);
         digitalWrite(LED_G2, LOW);
         displayNumber(i);
         }  
       }
       clearDisplay();
       digitalWrite(LED_R, LOW);
       digitalWrite(LED_G2, LOW);
       digitalWrite(Buzz_pin, LOW);

     for (int i = 3; i >= 0; i--){
         for(int j = 0; j<100; j++){
            digitalWrite(Buzz_pin, HIGH);
            digitalWrite(LED_Y2, HIGH);
            displayNumber(i);
           }
       }
      digitalWrite(Buzz_pin, LOW);   
      digitalWrite(LED_Y2, LOW);

      for (int i = t_G; i >= 4; i--) { 
        for(int j = 0; j<100; j++){
         digitalWrite(LED_G, HIGH);
         digitalWrite(LED_R2, HIGH);
         displayNumber(i);
         }
       } 
      for (int i = 3; i >= 0; i--) { 
        digitalWrite(Buzz_pin, HIGH);
        for(int j = 0; j<50; j++){
         digitalWrite(LED_G, HIGH);
         digitalWrite(LED_R2, HIGH);
         displayNumber(i);
         }
      
        for(int j = 0; j<50; j++){
         digitalWrite(LED_G, LOW);
         digitalWrite(LED_R2, LOW);
         displayNumber(i);      
         } 
      }
      clearDisplay();// clear display is necessary to reset the timer
     digitalWrite(LED_G, LOW);
     digitalWrite(LED_R2, LOW);
     digitalWrite(Buzz_pin, LOW);

     for (int i = 3; i >= 0; i--) { 
        for(int j = 0; j<100; j++){
          digitalWrite(Buzz_pin, HIGH);
          digitalWrite(LED_Y, HIGH);
         displayNumber(i);
         }
       }
     digitalWrite(Buzz_pin, LOW);  
     digitalWrite(LED_Y, LOW);
     clearDisplay();
     }
 
     }   
     
   else {
      if (n < 3) {
         input_key[n] = customKey;// keypad's input will be saved here 
         Serial.println(customKey); 
         n++; 
        if (flag2 == 1) {// this loops helps keypad's input to save the letter and number into the array. with time interrupt as 0.2 seconds 
         flag2 = 0; // Clear the flag
         if (toggle2 == 1) {// this loops is time interrupt as 0.2 seconds       
           toggle2 = 0;
           } 
         else {     
           toggle2 = 1;
           }
         }
      }
    }
         
   

 }
}

void displayNumber(int num) {// This is the function for displaying number from the 4 byte 7 segements.
  int digits[2];
  digits[0] = num / 10;// 10's digit number
  digits[1] = num % 10;//1's digit number
  
  if(num>9){
  for (int i = 0; i < 2; i++) {     
     digitalWrite(digitPins[i], HIGH);
     shiftOut(dataPin, clockPin, MSBFIRST, segmentPins[digits[i]]);
     digitalWrite(latchPin, HIGH);
     for(int i=0; i<8400; i++){// time interrupt with for loops make time interrupts as 5 miliseconds clearly.
       if (flag3 == 1) { 
        flag3 = 0; 
        if (toggle3 == 1) {      
          toggle3 = 0;         
         } else {}
        }
      }
    digitalWrite(latchPin, LOW);
    digitalWrite(digitPins[i], LOW);
  }
  }
  if(num<=9){   
  for (int i = 0; i < 2; i++) {   
     digitalWrite(digitPins[1], HIGH);
     shiftOut(dataPin, clockPin, MSBFIRST, segmentPins[digits[1]]);
     digitalWrite(latchPin, HIGH);
     for(int i=0; i<8400; i++){// time interrupt with for loops make time interrupts as 5 miliseconds clearly.
       if (flag3 == 1) { 
        flag3 = 0; 
        if (toggle3 == 1) {      
          toggle3 = 0;         
         } else {}
        }
      }
   
    digitalWrite(latchPin, LOW);
    digitalWrite(digitPins[1], LOW);
  }
  }
} 
 

void clearDisplay() {
  shiftOut(dataPin, clockPin, MSBFIRST, 0x00);
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}