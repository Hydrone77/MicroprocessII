// EECEE 5250 Lab 1
// Winflash Hwang
// 10/08/2024

#include <avr/io.h>
#include <avr/interrupt.h>

// LED pins
const int led1 = 12; // Blink every 1 second
const int led2 = 11; // Blink every 0.2 seconds
const int led3 = 10; // Blink every 5 milliseconds

volatile int flag1 = 0; // Use int for flag
volatile int flag2 = 0; // Use int for flag
volatile int flag3 = 0; // Use int for flag
volatile int toggle1 = 0; // Use int for toggle
volatile int toggle2 = 0; // Use int for toggle
volatile int toggle3 = 0; // Use int for toggle

void setup() {
  // Set pins as outputs
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);

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
  OCR2A = 124; // = (16*10^6) / (64 * 200) - 1
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

void handleLED3() {
  if (flag3 == 1) { // Check if the flag is set
    flag3 = 0; // Clear the flag
    if (toggle3 == 1) {
      digitalWrite(led3, HIGH);
      
      toggle3 = 0;
    } else {
      digitalWrite(led3, LOW);
      toggle3 = 1;
    }
  }
}

void loop() {
  if (flag1 == 1) { // Check if the flag is set
    flag1 = 0; // Clear the flag
    if (toggle1 == 1) {
      digitalWrite(led1, HIGH);
      toggle1 = 0;
    } else {
      digitalWrite(led1, LOW);
      toggle1 = 1;
    }
  }

  if (flag2 == 1) { // Check if the flag is set
    flag2 = 0; // Clear the flag
    if (toggle2 == 1) {
      digitalWrite(led2, HIGH);
      toggle2 = 0;
    } else {
      digitalWrite(led2, LOW);
      toggle2 = 1;
    }
  }

  handleLED3(); // Call the function to handle LED3
}


