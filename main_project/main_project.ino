
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
#include <SPI.h>
#include <MFRC522.h>                    // RFID library
#include <Servo.h>                      // Servo library
#include <Stepper.h>                    // Motor library
#include <DS1302.h>                     // Clock library

// RFID
#define RST_PIN 9                       // Configurable, see typical pin layout above
#define SS_PIN 10                       // Configurable, see typical pin layout above

// PROXIMITY SENSOR
#define sensorPin A0

// MOTOR
#define STEPSPERREVOLUTION 4096
#define STEPSTOFEED 2048

#define IN1 5
#define IN2 6
#define IN3 7
#define IN4 8

#define SERVOPIN 19


Stepper myStepper(STEPSPERREVOLUTION, IN1, IN2, IN3, IN4);    // Create motor instance

MFRC522 mfrc522(SS_PIN, RST_PIN);       // Create RFID instance

Servo servoCover;                       // Create servo instance

int coverPos = 0;                       // Variable to store the servo position

bool open = false;                      // Boolean to control the cover

// MODULO DE RELOJ
namespace {
// Set the appropriate digital I/O pin connections. These are the pin
// assignments for the Arduino as well for as the DS1302 chip. See the DS1302
// datasheet:   http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
const int kCePin   = 4;  // Chip Enable
const int kIoPin   = 3;  // Input/Output
const int kSclkPin = 2;  // Serial Clock

// Create a DS1302 object.
DS1302 rtc(kCePin, kIoPin, kSclkPin);

String dayAsString(const Time::Day day) {
  switch (day) {
    case Time::kSunday: return "Sunday";
    case Time::kMonday: return "Monday";
    case Time::kTuesday: return "Tuesday";
    case Time::kWednesday: return "Wednesday";
    case Time::kThursday: return "Thursday";
    case Time::kFriday: return "Friday";
    case Time::kSaturday: return "Saturday";
  }
  return "(unknown day)";
}

void printTime() {
  // Get the current time and date from the chip.
  Time t = rtc.time();

  // Name the day of the week.
  const String day = dayAsString(t.day);

  // Format the time and date and insert into the temporary buffer.
  char buf[50];
  snprintf(buf, sizeof(buf), "%s %04d-%02d-%02d %02d:%02d:%02d",
           day.c_str(),
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);

  // Print the formatted string to serial so we can see the time.
  Serial.println(buf);
}

}  // namespace


void setup() {

  Serial.begin(9600);               // Init the serial port
  SPI.begin();                      // Init SPI bus

  mfrc522.PCD_Init();               // Init MFRC522 card

  servoCover.attach(SERVOPIN);             // Attaches the servo on pin 7 to the servo object

  pinMode(sensorPin, INPUT);        // Sensor pin -> input

  Serial.println(F("Waiting for RFID card"));  //Shows in serial that the RFID is ready to read

  myStepper.setSpeed(5);            // Set speed to the motor

  // Initialize a new chip by turning off write protection and clearing the
  // clock halt flag. These methods needn't always be called. See the DS1302
  // datasheet for details.
  rtc.writeProtect(false);
  rtc.halt(false);

  // Make a new time object to set the date and time.
  Time t(2023, 4, 19, 17, 34, 0, Time::kSaturday);

  rtc.time(t);                        // Set the time and date on the chip.
}

void loop() {

  Time actualTime = rtc.time();       // Get the actual time
  //printTime();
  char buf[50];
  snprintf(buf, sizeof(buf), "Segundos: %d", actualTime.sec);     // Shows in serial the seconds 
  Serial.println(buf);

  // If its time to feed, the motor moves 
  if(actualTime.sec == 0){
    Serial.println("Girando 360");
    myStepper.step(2048);
    //delay(2000);
  }

  //------- IDLE STATE -------
  if (!mfrc522.PICC_IsNewCardPresent() && !open) {
    return;           // NO TAG DETECTED
  }

  ////------- OPEN STATE (TAG DETECTED)-------
  open = true;
  // Moves the servo to open the cover
  while (coverPos-- >= 0) {
    Serial.println("Servo opening");
    servoCover.write(coverPos);
    delay(15);
  }

  Serial.println("opening");

  // We find out if a cat is eating (Sensor >= 970 -> cat eating)
  int sensorValue;
  while((sensorValue = analogRead(sensorPin)) >= 970) {
    Serial.println(sensorValue);
    delay(100);
  }

  // Theres no cat, servo closing
  Serial.println("closing");
  while (coverPos++ <= 180) {  // goes from 0 degrees to 180 degrees
    servoCover.write(coverPos);  // tell servo to go to position in variable 'pos'
    delay(15);  // waits 15 ms for the servo to reach the position
  }

  open = false;     // The over is closed

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(100);
}

