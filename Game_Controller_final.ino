
#include <Wire.h>
#include <MPU9250_asukiaaa.h>

MPU9250_asukiaaa mySensor;

#define VRX_PIN  A0 // Arduino pin connected to VRX pin
#define VRY_PIN  A1 // Arduino pin connected to VRY pin

#define LEFT_THRESHOLD  400
#define RIGHT_THRESHOLD 800
#define UP_THRESHOLD    400
#define DOWN_THRESHOLD  800

#define COMMAND_NO     0x00
#define COMMAND_LEFT   0x01
#define COMMAND_RIGHT  0x02
#define COMMAND_UP     0x04
#define COMMAND_DOWN   0x08
#define DOUBLE         0x07


int xValue = 0 ; // To store value of the X axis
int yValue = 0 ; // To store value of the Y axis
int command = COMMAND_NO;

int buzzPin = 13;//Active Buzzer

void setup() {
  Serial.begin(9600) ;

  mySensor.setWire(&Wire);
  mySensor.beginAccel();
  mySensor.beginGyro();
  mySensor.beginMag();

  Serial.println("MPU9250 initialized successfully!");

  pinMode(buzzPin, OUTPUT);
}

void loop() {
   
  // read analog X and Y analog values
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);
 

  mySensor.accelUpdate();
  mySensor.gyroUpdate();
  mySensor.magUpdate();

  // converts the analog value to commands
  // reset commands
  command = COMMAND_NO;
  
  float x_gy = mySensor.gyroX();
  float y_gy = mySensor.gyroY();
  float z_gy = mySensor.gyroZ();
  float x_accel = mySensor.accelX(); 
  float y_accel = mySensor.accelY(); 
  float z_accel = mySensor.accelZ();

  // check left/right commands
  if (xValue < LEFT_THRESHOLD || y_gy < -80.00 ){
    command = COMMAND_RIGHT;
   }
  else if (xValue > RIGHT_THRESHOLD || y_gy > 80.00){
    command = COMMAND_LEFT;
   }
  // check up/down commands
  if (yValue < UP_THRESHOLD ||x_gy > 80.00 ){
   command = COMMAND_UP;
   }
  else if (yValue > DOWN_THRESHOLD || x_gy < -80.00 ){
    command = COMMAND_DOWN;
   }
  if (abs(x_accel) > 2.0 || abs(y_accel) > 2.0 || abs(z_accel) > 2.0) { 
   command = DOUBLE;
   }
  
  // NOTE: AT A TIME, THERE MAY BE NO COMMAND, ONE COMMAND OR TWO COMMANDS
  Serial.write(command);
  delay(200);

 if (Serial.available() > 0) { 
    String flag = Serial.readStringUntil('\n'); 
    if (flag == "flag=eat") {       
     tone(buzzPin, 5000, 200); // Beep for 200 ms
     }  
  }
}



