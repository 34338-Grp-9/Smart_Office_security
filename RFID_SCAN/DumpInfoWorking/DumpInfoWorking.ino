/*
  Sketch generated by the Arduino IoT Cloud Thing "Untitled"
  https://create.arduino.cc/cloud/things/c16bd2b7-d009-4a8f-803f-fb1a38481c72

  Arduino IoT Cloud Variables description

  The following variables are automatically generated and updated when changes are made to the Thing

  bool activateSecurity;
  bool denyEntry;

  Variables which are marked as READ/WRITE in the Cloud Thing will also have functions
  which are called when their values are changed from the Dashboard.
  These functions are generated with the Thing and added at the end of this sketch.
*/

#include "chipThingProperties.h"
#include "SPI.h"
#include "MFRC522.h"
#include "Servo.h"

#define RST_PIN D1   // D1    // Configurable, see typical pin layout above
#define SS_PIN D2    // D2 // Configurable, see typical pin layout above
#define BUZZER D8    // D8
#define SERVO_PIN D4 // D4
#define LED D0

unsigned long previousMillis = 0;
unsigned long interval = 1000;

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
Servo servo;
NTPClient timeClient(ntpUDP, "dk.pool.ntp.org", UTC_PLUS_ONE);
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
Firebase firebase(REFERENCE_URL); // firebase instance
String UID, name;
int value_clock;

void setup()
{
  // Initialize serial and wait for port to open:
  Serial.begin(115200);
  // This delay gives the chance to wait for a Serial Monitor without blocking if none is found
  delay(1500);

  // Defined in thingProperties.h
  initProperties();

  // Inititilize MFRC
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522

  pinMode(BUZZER, OUTPUT);
  servo.attach(SERVO_PIN);
  pinMode(LED, OUTPUT);
  servo.write(0);
  mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
  Serial.println("Bro, det virker");
  // Connect to Arduino IoT Cloud
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  /*
     The following function allows you to obtain more information
     related to the state of network and IoT Cloud connection and errors
     the higher number the more granular information you’ll get.
     The default is 0 (only errors).
     Maximum is 4
  */
  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();
}

void loop()
{
  ArduinoCloud.update();
  // Your code here
  DOOR_OPEN();
  timeClient.update();
}

void DOOR_OPEN()
{
  unsigned long currentMillis = millis();

  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return;
  }

  Serial.print("UID tag :");
  String content = "";

  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  if ((content.substring(1) == "A1 C5 D9 1D") && !denyEntry)
  {
    // Card with matching UID
    if (currentMillis - previousMillis >= interval)
    {
      // save the last time you blinked the LED
      previousMillis = currentMillis;
      tone(Buzzer, 440, 500); // Play tone
      user_scan = "Login: A1 C5 D9 1D: Access Granted";
      ledAccess();
    }
  }
  else
  {
    // Card with non-matching UID
    tone(Buzzer, 200, 500);
    user_scan = "Access denied";
  }
}

/*
  Since ActivateSecurity is READ_WRITE variable, onActivateSecurityChange() is
  executed every time a new value is received from IoT Cloud.
*/
void onActivateSecurityChange()
{
  // Add your code here to act upon ActivateSecurity change
  Serial.print("deny Entry change: ");
  if (activateSecurity)
  {
    denyEntry = true;
  }
  else
  {
    denyEntry = false;
  }
  Serial.println(denyEntry);
  // (activateSecurity) ? servo.write(130) : servo.write(0) ;
}

void ledAccess()
{
  lEDSTATE = HIGH;
  digitalWrite(LED, lEDSTATE);
  servo.write(130); // Make servo go to 0 degrees
  delay(5000);
  servo.write(0); // Make servo go to 180 degrees
  lEDSTATE = LOW;
  digitalWrite(LED, lEDSTATE);
  delay(1000);
}
