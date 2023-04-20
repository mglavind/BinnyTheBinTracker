#include <Arduino.h>


// Begin setup for Wireless communitcation
#define DummyVariable
// End setup for Wireless communitcation


// Begin setup for Sensors
const int trigPin = 12;
const int echoPin = 14;
//define sound velocity in cm/uS (cm pr. microsecond)
#define SOUND_VELOCITY 0.034
long duration;
float distanceCm;
// End setup for Sensors


// Begin setup for RFID

#include <MFRC522.h> //library responsible for communicating with the module RFID-RC522
#include <SPI.h> //library responsible for communicating of SPI bus
#define SS_PIN    5
#define RST_PIN   22
#define SIZE_BUFFER     18
#define MAX_SIZE_BLOCK  16
#define greenPin     12
#define redPin       32

// End setup for  RFID


// Begin setup for MQTT

#include <WiFi.h>
#include <MQTT.h>

const char ssid[] = "ssid";
const char pass[] = "pass";

const char MqttID [] = "arduino" ;
const char MqttUsername [] = "public" ;
const char MqttPassword [] = "public" ;




WiFiClient net;
MQTTClient client;

unsigned long lastMillis = 0;

// End setup for 


  //////////////////////////////////////
 ///////* Everything Sensor *//////////
//////////////////////////////////////

void InitDistanceSensor() {
  // wite stuff to initialize the sensor here
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void GetDistance() {
  // wite stuff to initialize the sensor here
    // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculate the distance
  distanceCm = duration * SOUND_VELOCITY/2;
  // Prints the distance on the Serial Monitor
  Serial.print("Distance (cm): ");
  Serial.println(distanceCm);
}

void ReadSensor2() {
  // wite stuff to initialize the sensor here
}

void ReadSensor3() {
  // wite stuff to initialize the sensor here
}

  ////////////////////////////////////////
 ///////* Everything Wireless *//////////
////////////////////////////////////////

void WirelessInitialization() {
  // wite stuff to initialize the wireless communication here
}

void WirelessTX() {
  // wite stuff to initialize the wireless communication here
}
void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);

  // Note: Do not use the client in the callback to publish, subscribe or
  // unsubscribe as it may cause deadlocks when other things arrive while
  // sending and receiving acknowledgments. Instead, change a global variable,
  // or push to a queue and handle it in the loop after calling `client.loop()`.
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(MqttID, MqttUsername, MqttPassword)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe("/hello");
  // client.unsubscribe("/hello");
}


  /////////////////////////////////////////////
 ///////* Everything Låsemekanisme *//////////
/////////////////////////////////////////////

void LockInitialization() {
  // wite stuff to initialize the wireless communication here
}

void LockA() {
  // wite stuff to initialize the wireless communication here
}
void LockB() {
  // wite stuff to initialize the wireless communication here
}







  ///////////////////////////////////
 ///////* The main code *///////////
///////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Starts the serial communication
  //////////////////////////////////////////////////////////////////////////////////////////// DistanceSensor

  InitDistanceSensor();

  //////////////////////////////////////////////////////////////////////////////////////////// DistanceSensor
  
  
  //////////////////////////////////////////////////////////////////////////////////////////// MQTT
  WiFi.begin(ssid, pass);

  // Note: Local domain names (e.g. "Computer.local" on OSX) are not supported
  // by Arduino. You need to set the IP address directly.
  //
  // MQTT brokers usually use port 8883 for secure connections.
  client.begin("public.cloud.shiftr.io", 8883, net);
  client.onMessage(messageReceived);

  connect();
  //////////////////////////////////////////////////////////////////////////////////////////// MQTT

}

void loop() {
  // put your main code here, to run repeatedly:





  //////////////////////////////////////////////////////////////////////////////////////////// MQTT
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  if (millis() - lastMillis > 1000) {
    lastMillis = millis();
    client.publish("/hello", "world");
  }
  //////////////////////////////////////////////////////////////////////////////////////////// MQTT


}


