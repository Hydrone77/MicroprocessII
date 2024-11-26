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

void setup() {
  Serial.begin(115200); // Baud rate for the Serial Monitor
  pinMode(MIC_PIN, INPUT);
  samplingPeriod = round(1000000 * (1.0 / SAMPLING_FREQUENCY)); // Period in microseconds
}

void loop() {
  // Sampling
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

  // Find peak frequency
  double peakFrequency = FFT.majorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);

  // Get the current time in milliseconds
  unsigned long currentMillis = millis();
  
  // Check if the current time is greater than the previous time by the interval
  
    
    // Print the detected frequency
    Serial.println(vReal[50]);
    Serial.println("\n");
    
  
}


