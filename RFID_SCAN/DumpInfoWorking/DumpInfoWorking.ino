/*
Just a comment to remember where the pins are suppose to be
RST = D1
SPI ss = D2
SPI MOSI  = D7
SPI MISO = D6
SPI SCK = D5
*/
#include <ESP8266Firebase.h>  //Library by https://github.com/Rupakpoddar/ESP8266Firebase
#include <ESP8266WiFi.h>
#include <NTPClient.h>  //Library by https://github.com/arduino-libraries/NTPClient/
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFiUdp.h>

//Our electronics
#define RST_PIN D1   //D1
#define SS_PIN D2    //D2
#define BUZZER D8    //D8
#define SERVO_PIN 2  //D4
#define LED D0       //D0

//Wifi
#define SSID "simsek"                                                                        // SSID
#define PASSWORD "Yavuz112"                                                                  // Password
#define REFERENCE_URL "https://proj1-2189f-default-rtdb.europe-west1.firebasedatabase.app/"  // Firebase reference URL




int LEDSTATE = 0;
unsigned long previousMillis = 0;
unsigned long interval = 1000;

//UTC+1
const long UTC_PLUS_ONE = 3600;


WiFiUDP ntpUDP;
Servo servo;
NTPClient timeClient(ntpUDP, "dk.pool.ntp.org", UTC_PLUS_ONE);
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
Firebase firebase(REFERENCE_URL);  //firebase instance
String UID, name;
int value_clock;

void setup() {
  Serial.begin(115200);
  SPI.begin();         // Init SPI bus
  mfrc522.PCD_Init();  // Init MFRC522
  pinMode(BUZZER, OUTPUT);
  servo.attach(SERVO_PIN);
  pinMode(LED, OUTPUT);
  servo.write(0);

  // Connect to WiFi
  Serial.println();
  Serial.println();
  Serial.print("Connecting to: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  timeClient.begin();

  // Print the IP address
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  DOOR_OPEN();
  timeClient.update();
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
  UID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    UID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : ""));
    UID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  UID.toUpperCase();

  name = firebase.getString(UID + "/Name");
  value_clock = firebase.getInt(UID + "/Clock");

  /*
  ul is what firebase registeres when a scanned UID is not from the database. 
  the getString calls out our databse to check if the scanned uid is not ul and from the database.
  */
  Serial.print("Querying for UID: ");
  Serial.println(UID);
  if (firebase.getString(UID) != "ul" && value_clock == 0) {

    firebase.setInt(UID + "/Clock", 1);
    firebase.pushString("Timestamp/" + name + "/Clockin", timeClient.getFormattedTime());
    Serial.println(name);
    Serial.println("Logged ind");

    if (currentMillis - previousMillis >= interval) {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      tone(BUZZER, 440, 500);  // Play tone

      // set the LED with the ledState of the variable:
      digitalWrite(LED, LEDSTATE);
      servo.write(180);  // Make servo go to 180 degrees
      delay(5000);
      servo.write(0);  // Make servo go to 0 degrees
      LEDSTATE = LOW;
      digitalWrite(LED, LEDSTATE);
      // if the LED is off turn it on and vice-versa:
      if (LEDSTATE == LOW) {
        LEDSTATE = HIGH;
      } else {
        LEDSTATE = LOW;
      }
    }


  } else if (value_clock == 1) {
    firebase.pushString("Timestamp/" + name + "/Clockout", timeClient.getFormattedTime());
    firebase.setInt(UID + "/Clock", 0);
    Serial.println(name);
    Serial.println("Logged out");


  } else {
    // Card with non-matching UID
    Serial.print(UID);
    Serial.println(", Does not exist");
    tone(BUZZER, 200, 500);
  }
}
