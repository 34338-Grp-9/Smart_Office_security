#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>  // Include the Wi-Fi-Multi library
#include <ESP8266WebServer.h>  // Include the WebServer library
#include <ESP8266mDNS.h>       // Include the mDNS library
#include <Servo.h>             // Include servo motor library


ESP8266WiFiMulti wifiMulti;
// Create an instance of the server
ESP8266WebServer server(80);

const int led = D2;
const int servoMotorPin = D8;

void handleRoot();
void handleLED();
void handleNotFound();

// Servo motor for simulating office door
Servo door;
const int closedDoor = 0;
const int openedDoor = 130;


void setup() {
  Serial.begin(115200);
  delay(10);

/*
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);
*/
// Init servo motor
  pinMode(servoMotorPin, OUTPUT);
  door.attach(servoMotorPin);
  door.write(closedDoor);
  
  // Connect to WiFi network
  Serial.println();
  wifiMulti.addAP("OnePlus 7T Inc.", "12345ABCDE!!");  // add Wi-Fi networks you want to connect to
  wifiMulti.addAP("<ssid2>", "<password>");

  Serial.println();
  Serial.print("Connecting ...");
  //WiFi.begin(ssid, password);

  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected to ");
  Serial.println(WiFi.SSID());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("iot")) {  // Start the mDNS responder for esp8266.local
    Serial.println("mDNS responder started");
  } else {
    Serial.println("Error setting up MDNS responder!");
  }



  server.on("/", HTTP_GET, handleRoot);
  //server.on("/LED", HTTP_POST, handleLED);
  server.on("/DOOR", HTTP_POST, handleDoor);
  server.onNotFound(handleNotFound);

  
 
  
  // Start the server
  server.begin();
  Serial.println("Server started");


}

void loop() {
  // Check if a client has connected
  server.handleClient();
  
}



void handleDoor() {
  //Change this to interact with access card
  (door.read()==0) ? openDoor() : closeDoor() ;
  
  server.sendHeader("Location", "/");    // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);
  Serial.println("Done handling");
}

//Change this to interact with access card
void openDoor() {
  Serial.println("TESTING OPEN");
  door.write(openedDoor);
}

//Change this to interact with access card
void closeDoor() {
  Serial.println("TESTING CLOSE");
  door.write(closedDoor);
}

// Håndtere html siden for vores webserver
void handleRoot() {  // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html",
              "<html><title>Internet of Things - Demonstration</title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>Velkommen til denne WebServer</h1> \ 
      <p>Internet of Things (IoT) er \"tingenes Internet\" - dagligdags ting kommer på nettet og får ny værdi. Det kan løse mange udfordringer.</p> \
      <p>Her kommunikerer du med en webserver på en lille microcontroller af typen Arduino, som i dette tilfælde styrer en digital udgang, som du så igen kan bruge til at styre en lampe, en ventilator, tænde for varmen eller hvad du lyster</p> \
      <p>Klik på nedenstående knap for at tænde eller slukke LED på port D2</p> \
      <form action=\"/DOOR\" method=\"POST\"><input type=\"submit\" value=\"Skift tilstand på kontor dør\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      <p>Med en Arduino ESP8266 kan du lave et have a sjove projekter</p> \
      <p>Vil du vide mere: Kig på hjemmesiden for uddannelsen : <a href=\"www.dtu.dk/net\">Netværksteknologi og it</a></p> \
      </body></html>");
}

/*
//This is for LED button and Servo motor
void handleRoot() {  // When URI / is requested, send a web page with a button to toggle the LED
  server.send(200, "text/html",
              "<html><title>Internet of Things - Demonstration</title><meta charset=\"utf-8\" \/> \ 
      </head><body><h1>Velkommen til denne WebServer</h1> \ 
      <p>Internet of Things (IoT) er \"tingenes Internet\" - dagligdags ting kommer på nettet og får ny værdi. Det kan løse mange udfordringer.</p> \
      <p>Her kommunikerer du med en webserver på en lille microcontroller af typen Arduino, som i dette tilfælde styrer en digital udgang, som du så igen kan bruge til at styre en lampe, en ventilator, tænde for varmen eller hvad du lyster</p> \
      <p>Klik på nedenstående knap for at tænde eller slukke LED på port D2</p> \
      <form action=\"/LED\" method=\"POST\" ><input type=\"submit\" value=\"Skift tilstand på LED\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      <form action=\"/DOOR\" method=\"POST\"><input type=\"submit\" value=\"Skift tilstand på kontor dør\" style=\"width:500px; height:100px; font-size:24px\"></form> \
      <p>Med en Arduino ESP8266 kan du lave et have a sjove projekter</p> \
      <p>Vil du vide mere: Kig på hjemmesiden for uddannelsen : <a href=\"www.dtu.dk/net\">Netværksteknologi og it</a></p> \
      </body></html>");
}*/

void handleLED() {                       // If a POST request is made to URI /LED
  digitalWrite(led, !digitalRead(led));  // Change the state of the LED
  server.sendHeader("Location", "/");    // Add a header to respond with a new location for the browser to go to the home page again
  server.send(303);                      // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");  // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
