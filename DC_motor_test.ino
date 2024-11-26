#define ENABLE 3  // PWM-capable pin
#define DIRA 5
#define DIRB 4
#define buttonPin 13 // pin of the switch button 

int buttonState = 0;
int speed = 100;  // PWM duty cycle value (0-255)
int DIRAstate = HIGH;
int DIRBstate = LOW;
void setup() {
  pinMode(ENABLE, OUTPUT);  // Set ENABLE pin as output
  pinMode(DIRA, OUTPUT);    // Set DIRA pin as output
  pinMode(DIRB, OUTPUT);    // Set DIRB pin as output
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);       // Initialize serial communication
}

void loop() {
  buttonState = digitalRead(buttonPin); 
  
  if(buttonState == LOW) {
    Serial.println("The switch has been pressed");
    if(DIRAstate == HIGH && DIRBstate == LOW){
      Serial.println("Counter Clockwise rotation");
      DIRAstate = LOW;
      DIRBstate = HIGH;
      analogWrite(ENABLE, 0);
      delay(500);  
    }
   else if(DIRAstate == LOW && DIRBstate == HIGH){
    Serial.println("Clockwise rotation");
      DIRAstate = HIGH;
      DIRBstate = LOW;
      analogWrite(ENABLE, 0);
      delay(500);  
    }
  }

else{  
  digitalWrite(DIRA, DIRAstate);  // Set motor direction clockwise
  digitalWrite(DIRB, DIRBstate);
  analogWrite(ENABLE, speed);  
} 
}
