
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
#include <MFRC522.h>
#include <Servo.h>
#include <Stepper.h>
#include <DS1302.h>

#define RST_PIN 3  // Configurable, see typical pin layout above
#define SS_PIN 4  // Configurable, see typical pin layout above
#define sensorPin A0
#define STEPSPERREVOLUTION 4096
#define STEPSTOFEED 2048

Stepper myStepper(STEPSPERREVOLUTION, 8, 9, 10, 11);
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

Servo servoCover;  // create servo object to control a servo

int coverPos = 0;  // variable to store the servo position

bool open = false;
namespace {

// Set the appropriate digital I/O pin connections. These are the pin
// assignments for the Arduino as well for as the DS1302 chip. See the DS1302
// datasheet:
//
//   http://datasheets.maximintegrated.com/en/ds/DS1302.pdf
const int kCePin   = 5;  // Chip Enable
const int kIoPin   = 6;  // Input/Output
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
  // initialize the serial port:
  Serial.begin(9600);
  SPI.begin();                      // Init SPI bus
  mfrc522.PCD_Init();               // Init MFRC522 card
  servoCover.attach(7);               // attaches the servo on pin 9 to the servo object
  pinMode(sensorPin, INPUT);
  Serial.println(F("Waiting for RFID card"));  //shows in serial that it is ready to read
  myStepper.setSpeed(5);
  // Initialize a new chip by turning off write protection and clearing the
  // clock halt flag. These methods needn't always be called. See the DS1302
  // datasheet for details.
  rtc.writeProtect(false);
  rtc.halt(false);

  // Make a new time object to set the date and time.
  // Sunday, September 22, 2013 at 01:38:50.
  Time t(2023, 4, 19, 17, 34, 0, Time::kSaturday);

  // Set the time and date on the chip.
  rtc.time(t);
}

void loop() {

  Time actualTime = rtc.time();
  //printTime();
  char buf[50];
  snprintf(buf, sizeof(buf), "hola %d", actualTime.sec);
  Serial.println(buf);
  if(actualTime.sec == 0){
    Serial.println("Girando 360");
    myStepper.step(2048);
    //delay(2000);
  }
  //IDLE
  if (!mfrc522.PICC_IsNewCardPresent() && !open) {
    return;
  }
   //OPEN
  open = true;
  while (coverPos-- >= 0) {
    Serial.println("Servo opening");
    servoCover.write(coverPos);
    delay(15);
  }
  Serial.println("opening");
  int sensorValue;
  while((sensorValue = analogRead(sensorPin)) >= 970) {
    Serial.println(sensorValue);
    delay(100);
  }
  Serial.println("closing");
  while (coverPos++ <= 180) {  // goes from 0 degrees to 180 degrees
    servoCover.write(coverPos);  // tell servo to go to position in variable 'pos'
    delay(15);  // waits 15 ms for the servo to reach the position
  }
  open = false;

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(100);
}

