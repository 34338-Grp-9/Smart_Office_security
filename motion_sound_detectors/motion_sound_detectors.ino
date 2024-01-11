// millis for timing purposes
unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 500;

// things for PIR motion sensor
int sensor = D4;          // motion sensor pin
int currentState   = LOW; // current state of sensor pin
int previousState  = LOW; // previous state of sensor pin

// things for HW484 sound sensor
const int LEDpin1 = D8;   // RBGLED1
const int LEDpin2 = D7;   // RBGLED2
const int LEDpin3 = D6;   // RBGLED3
int raw_Value = A0;       // Analog sound input

//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;  

void setup() {
  Serial.begin(9600);
  pinMode(sensor, INPUT)  ;    // initialize sensor as an input
  pinMode(raw_Value, INPUT);   // Set pin A0 as an input
  pinMode(LEDpin1, OUTPUT);    // RBGLED1 as output
  pinMode(LEDpin2, OUTPUT);    // RBGLED2 as output
  pinMode(LEDpin3, OUTPUT);    // RBGLED3 as output
  startMillis = millis();

  //give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
    for(int i = 0; i < calibrationTime; i++){
      Serial.print(".");
      delay(1000);
      }
    Serial.println(" done");
    Serial.println("SENSOR ACTIVE");
    delay(50);
}

void loop(){
  previousState = currentState; // store old motion state
  currentState = digitalRead(sensor); // read new motion state

  if (previousState == LOW && currentState == HIGH) { // turn on LED if motion detected
    Serial.println("Motion detected!");
    //digitalWrite(led, HIGH);
  }
  else
  if (previousState == HIGH && currentState == LOW) { // turn off LED if motion stops
    Serial.println("Motion stopped.");
    //digitalWrite(led, LOW);
  }

  int val_analog = analogRead(raw_Value); // analog value from sound sensor
  float average = 0;

  for (int i = 0; i < 5; i++) { //loop to make average of 5 measurements
    average += val_analog;
  }
  average /= 5; // calculate average sound level
  int LEDvalue1, LEDvalue2;
  currentMillis = millis(); //use millis to define how often change happens

  if (currentMillis - startMillis >= period) { //test whether the period has elapsed
    //Serial.print("Current volume level: ");
    //Serial.println(average);
   Serial.print("Current motion state: ");
   Serial.println(currentState);
      if (average <= 100) {
      // For sound level below 100 LED is green
       LEDvalue1 = 0;
       LEDvalue2 = 255;
      } else {
       // Above 100, LED goes red
       LEDvalue1 = 255;
       LEDvalue2 = 0;
  }
    // Write the values to the LED pins
    analogWrite(LEDpin1, LEDvalue1); // write to red
    analogWrite(LEDpin2, LEDvalue2); // write to green
    analogWrite(LEDpin3, 0); // blue is not used
    startMillis = currentMillis;
  }
}