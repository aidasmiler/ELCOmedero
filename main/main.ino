/*
 * Initial Author: ryand1011 (https://github.com/ryand1011)
 *
 * Reads data written by a program such as "rfid_write_personal_data.ino"
 *
 * See: https://github.com/miguelbalboa/rfid/tree/master/examples/rfid_write_personal_data
 *
 * Uses MIFARE RFID card using RFID-RC522 reader
 * Uses MFRC522 - Library
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
*/

#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN 3  // Configurable, see typical pin layout above
#define SS_PIN 4  // Configurable, see typical pin layout above
#define sensorPin A0

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

Servo myservo1;  // create servo object to control a servo

int pos1 = 0;  // variable to store the servo position

bool open = false;

//*****************************************************************************************//
void setup() {
  Serial.begin(9600);                                         // Initialize serial communications with the PC
  SPI.begin();                                                // Init SPI bus
  mfrc522.PCD_Init();                                         // Init MFRC522 card
  Serial.println(F("Read personal data on a MIFARE PICC:"));  //shows in serial that it is ready to read
  myservo1.attach(7);                                         // attaches the servo on pin 9 to the servo object
  pinMode(sensorPin, INPUT);
}

//*****************************************************************************************//
void loop() {

  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.

  //-------------------------------------------

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  //IDLE
  if (!mfrc522.PICC_IsNewCardPresent() && !open) {
    return;
  }

  //OPEN
  open = true;
  while (pos1-- >= 0) {
    myservo1.write(pos1);
    delay(15);
  }
  Serial.println("opening");
  int sensorValue;
  while((sensorValue = analogRead(sensorPin)) >= 950) {
    Serial.println(sensorValue);
    delay(1000);
  }
  Serial.println("closing");
  while (pos1++ <= 180) {  // goes from 0 degrees to 180 degrees
    myservo1.write(pos1);  // tell servo to go to position in variable 'pos'
    delay(15);  // waits 15 ms for the servo to reach the position
  }
  open = false;

  delay(100);  //change value if you want to read cards faster

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
