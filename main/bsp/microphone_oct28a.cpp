//Title: Microphone
//Description:
//  A simple program on making a mircophone with a sensitivity of 44 dB with noise going up to 50 dB.  
//  This will later needs to be modified with an actual microphone that has a higher sensitivity.
//  

#include <stdio.h>
#include <Arduino.h>
#include <math.h>

#define ADC_PIN 36           // ADC pin (GPIO36, ADC Channel 0)
#define VREF 3.3             // ESP32 ADC reference voltage
#define SAMPLE_RATE 10000000     // Sampling rate in Hz
#define SAMPLE_SIZE 4000      // Number of samples in a period
#define PERIOD_MS (SAMPLE_SIZE / SAMPLE_RATE) // Measurement period in ms

void setup() {
    Serial.begin(115200);
    delay(1000);           // Allow time for the serial monitor to initialize
    pinMode(ADC_PIN, INPUT);   // Set ADC pin as input
}

// Function to measure and calculate the decibel level based on the highest ADC reading
void measure_function() {
    int maxReading = 0;

    // Collect samples and track the maximum ADC value
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        int reading = analogRead(ADC_PIN);  // Read ADC sample
        if (reading > maxReading) {
            maxReading = reading;  // Update the maximum reading
        }
        delayMicroseconds(10000 / SAMPLE_RATE);  // Ensure consistent sampling rate
    }

    // Convert the highest ADC value to voltage
    float vout = (maxReading * VREF) / 4095.0;

    // Prevent log10(0) errors by clamping the minimum voltage
    if (vout < 0.01) {
        vout = 1; // Avoid log10 issues with small or zero values
    }

    // Calculate decibel level, microphone sensitivity is at 42+-2
    float decibel = 20.0 * log10(vout)+42;

    // Print the decibel value
    Serial.print("Decibel Level: ");
    Serial.println(decibel);

    // Check if decibel level exceeds threshold
    if (decibel > 50) {
        Serial.println("Warning: High noise detected!");
    }
}

void loop() {
    unsigned long startTime = millis();

    // Measure and calculate dB using the highest ADC reading
    measure_function();

    // Calculate elapsed time and wait for the rest of the period if necessary
    unsigned long elapsedTime = millis() - startTime;
    if (elapsedTime < PERIOD_MS) {
        delay(PERIOD_MS - elapsedTime);  // Adjust if loop finished early
    }
}