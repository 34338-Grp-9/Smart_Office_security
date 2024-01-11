/*
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
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

/*
Just a comment to remember where the pins are suppose to be

RST = 5 D1
SPI ss = 4 D2
SPI MOSI  = D7
SPI MISO = D6
SPI SCK = D5


*/


#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define RST_PIN D1  //D1    // Configurable, see typical pin layout above
#define SS_PIN D2   //D2 // Configurable, see typical pin layout above
#define Buzzer D8   //D8
#define SERVO_PIN 2
#define PIR_SENSOR D3

const int LED = D0;
int LEDSTATE = 0;
unsigned long previousMillis = 0;
unsigned long interval = 1000;



MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
Servo servo;

void setup() {
  Serial.begin(9600);  // Initialize serial communications with the PC
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522
  delay(4);            // Optional delay. Some board do need more time after init to be ready, see Readme¨
  pinMode(Buzzer, OUTPUT);
  servo.attach(SERVO_PIN);
  pinMode(LED, OUTPUT);
  servo.write(0);
  mfrc522.PCD_DumpVersionToSerial();  // Show details of PCD - MFRC522 Card Reader details
  Serial.println("Bro, det virker");
}

void loop() {
  DOOR_OPEN();
}


void DOOR_OPEN() {
  unsigned long currentMillis = millis();

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print("UID tag :");
  String content = "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if (content.substring(1) == "A1 C5 D9 1D") {
    // Card with matching UID
    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      tone(Buzzer, 440, 500);  // Play tone
      Serial.println("brooo");
      // if the LED is off turn it on and vice-versa:
      if (LEDSTATE == LOW) {
        LEDSTATE = HIGH;
      } else {
        LEDSTATE = LOW;
      }
      // set the LED with the ledState of the variable:
      digitalWrite(LED, LEDSTATE);
      servo.write(0);  // Make servo go to 0 degrees
      delay(5000);
      servo.write(180);  // Make servo go to 180 degrees
      LEDSTATE = LOW;
      digitalWrite(LED, LEDSTATE);
      delay(1000);
    }

  } else {
    // Card with non-matching UID
    tone(Buzzer, 200, 500);
  }
}
