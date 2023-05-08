
/*
 Stepper Motor Control - one revolution

 This program drives a unipolar or bipolar stepper motor.
 The motor is attached to digital pins 8 - 11 of the Arduino.

 The motor should revolve one revolution in one direction, then
 one revolution in the other direction.


 Created 11 Mar. 2007
 Modified 30 Nov. 2009
 by Tom Igoe

 */

#include <Stepper.h>

#define STEPSPERREVOLUTION 4096
#define STEPSTOFEED 2048

#define IN1 A1 
#define IN2 A2
#define IN3 A3
#define IN4 A4

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(STEPSPERREVOLUTION, IN1, IN2, IN3, IN4);    // Create motor instance

void setup() {
  // set the speed at 60 rpm:
  myStepper.setSpeed(5);            // Set speed to the motor
  // initialize the serial port:
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);             // Motor -> output
  pinMode(IN2, OUTPUT);             
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  // step one revolution  in one direction:
  Serial.println("clockwise");
  myStepper.step(STEPSPERREVOLUTION);
  delay(2000);
}

