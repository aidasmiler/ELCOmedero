/*
 Created Mar-May 2023
 by GR 6 ELCO 2023 : 
    	Isabel Salmerón Marazuela 
    	Sergio Ruano Martínez
    	Aida Rodríguez Pérez 
    	Paula Rodríguez Sánchez 
    	María Rincón Gaitero 
    	Álvaro Gómez Pavón 
 */
#include <SPI.h>                        // SPI library
#include <MFRC522.h>                    // RFID library
#include <Servo.h>                      // Servo library
#include <Stepper.h>                    // Motor library
#include <DS1302.h>                     // Clock library

// RFID
#define RST_PIN 9                       // RFID reset Pin
#define SS_PIN 10                       // RFID DAT Pin

// PROXIMITY SENSOR
#define sensorPin A0

// MOTOR
#define STEPSPERREVOLUTION 4096
#define STEPSTOFEED 2048

#define IN1 A1 
#define IN2 A2
#define IN3 A3
#define IN4 A4

#define SERVOPIN 5

#define UMBRAL 990

#define TIEMPO_COMIDA 10


Stepper myStepper(STEPSPERREVOLUTION, IN1, IN2, IN3, IN4);    // Create motor instance

MFRC522 mfrc522(SS_PIN, RST_PIN);       // Create RFID instance

Servo servoCover;                       // Create servo instance

int coverPos = 180;                       // Variable to store the servo position

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

  servoCover.attach(SERVOPIN);      // Attaches the servo on pin 7 to the servo object

  pinMode(sensorPin, INPUT);        // Sensor pin -> input

  pinMode(IN1, OUTPUT);             // Motor -> output
  pinMode(IN2, OUTPUT);             
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

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
  //if(actualTime.sec % TIEMPO_COMIDA == 0){
  if(actualTime.sec == TIEMPO_COMIDA){
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
  Serial.println("opening");

  /*while (coverPos-- >= 0) {
    Serial.println("Servo opening");
    servoCover.write(coverPos);
    delay(15);
  } */

  for (coverPos = 180; coverPos >= 0; coverPos -= 1) { // goes from 0 degrees to 180 degrees
  // in steps of 1 degree
    servoCover.write(coverPos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }

  // We find out if a cat is eating (Sensor >= UMBRAL -> cat eating)
  int sensorValue;
  while((sensorValue = analogRead(sensorPin)) >= UMBRAL) {
    Serial.println(sensorValue);
    delay(100);
  }
  Serial.println(sensorValue);
  // Theres no cat, servo closing
  Serial.println("closing");
 
  /*while (coverPos++ <= 180) {    // goes from 0 degrees to 180 degrees
    servoCover.write(coverPos);  // tell servo to go to position in variable 'pos'
    delay(15);                   // waits 15 ms for the servo to reach the position
  } */

  for (coverPos = 0; coverPos <= 180; coverPos += 1) { // goes from 180 degrees to 0 degrees
    servoCover.write(coverPos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15 ms for the servo to reach the position
  }

  open = false;     // The cover is closed

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  delay(100);
}

