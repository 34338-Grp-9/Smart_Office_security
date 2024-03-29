// Code generated by Arduino IoT Cloud, DO NOT EDIT.

#include <ArduinoIoTCloud.h>
#include <Arduino_ConnectionHandler.h>

const char DEVICE_LOGIN_NAME[]  = "c10de9ae-7084-4f89-919f-8fb08fe9c38c";

const char SSID[]               = SECRET_SSID;    // Network SSID (name)
const char PASS[]               = SECRET_OPTIONAL_PASS;    // Network password (use for WPA, or use as key for WEP)
const char DEVICE_KEY[]  = SECRET_DEVICE_KEY;    // Secret device password

void onActivateSecurityChange();
void onLEDSTATEChange();
void onOpenDoorChange();

String user_scan;
bool activateSecurity;
bool denyEntry;
bool lEDSTATE;
bool openDoor;

void initProperties(){

  ArduinoCloud.setBoardId(DEVICE_LOGIN_NAME);
  ArduinoCloud.setSecretDeviceKey(DEVICE_KEY);
  ArduinoCloud.addProperty(user_scan, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(activateSecurity, READWRITE, ON_CHANGE, onActivateSecurityChange);
  ArduinoCloud.addProperty(denyEntry, READ, ON_CHANGE, NULL);
  ArduinoCloud.addProperty(lEDSTATE, READWRITE, ON_CHANGE, onLEDSTATEChange);
  ArduinoCloud.addProperty(openDoor, READWRITE, ON_CHANGE, onOpenDoorChange);

}

WiFiConnectionHandler ArduinoIoTPreferredConnection(SSID, PASS);
