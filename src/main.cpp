#include <Arduino.h>

  //////////////////////////////////////
 /////////////* EEPROM *///////////////
//////////////////////////////////////

#include <EEPROM.h>

// Define the size of the EEPROM block to use
#define EEPROM_SIZE 2048

// Define the starting address of the data block
#define DATA_START_ADDRESS 0

// Define the number of UID records to store
#define NUM_UIDS 480 // Theoretical max = EEPROM_SIZE / UID_SIZE

// Define the size of each UID record
#define UID_SIZE 4

// Define the total size of the UID array
#define UID_ARRAY_SIZE (NUM_UIDS * UID_SIZE)

byte knownUIDs[NUM_UIDS][UID_SIZE] = {  // List of known UIDs
  {0x12, 0x34, 0x56, 0x78},
  {0xAB, 0xCD, 0xEF, 0x01}
};

void SaveUIDs() {
  // Write the UID array to EEPROM
  for (int i = 0; i < NUM_UIDS; i++) {
    for (int j = 0; j < UID_SIZE; j++) {
      EEPROM.write(DATA_START_ADDRESS + i * UID_SIZE + j, knownUIDs[i][j]);
    }
  }

  // Commit the changes to EEPROM
  EEPROM.commit();
}

void LoadUIDs() {
  // Read the UID array from EEPROM
  for (int i = 0; i < NUM_UIDS; i++) {
    for (int j = 0; j < UID_SIZE; j++) {
      knownUIDs[i][j] = EEPROM.read(DATA_START_ADDRESS + i * UID_SIZE + j);
    }
  }
}

  //////////////////////////////////////
 /////////////* EEPROM *///////////////
//////////////////////////////////////






  //////////////////////////////////////
 //////////////* RFID *////////////////
//////////////////////////////////////

#include <MFRC522.h> //library responsible for communicating with the module RFID-RC522
#include <SPI.h> //library responsible for communicating of SPI bus

int SS_PIN  =  5;
int RST_PIN  = 22;

int SIZE_BUFFER   =  18;
int MAX_SIZE_BLOCK = 16;
int greenPin =    12;
int redPin   =    32;

bool RemoveUID = false;
bool AddUID = false;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

byte uid[4] = {0x00, 0x00, 0x00, 0x00 };






// Function to print UID bytes in hexadecimal format
void printUID(byte* uid, byte uidSize) {
    for (byte i = 0; i < uidSize; i++) {
        if (uid[i] < 0x10) {
            Serial.print("0");  // Add leading zero if necessary
        }
        Serial.print(uid[i], HEX);
        Serial.print(" ");

    }
}

void StringToUID(String hexstring) {
    hexstring.remove(0,2);
    byte byteArray[4];
    for (int i = 0; i < hexstring.length(); i += 2) {
      uid[i/2] = (byte) strtol(hexstring.substring(i, i+2).c_str(), NULL, 16);
    }
    printUID(uid, 4);
}


void SetupRFID() {
    SPI.begin();             // Initialize SPI bus
    mfrc522.PCD_Init();      // Initialize MFRC522
}

bool IsUIDKnown(byte uid[]) {
  for (int i = 0; i < 200; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (knownUIDs[i][j] != uid[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      // Found a match, UID is known
      return true;
    }
  }
  // UID not found in array
  return false;
}

  //////////////////////////////////////
 //////////////* RFID *////////////////
//////////////////////////////////////







  //////////////////////////////////////
 ///////* Everything Sensor *//////////
//////////////////////////////////////

// Begin setup for Sensors
const int trigPin = 12;
const int echoPin = 14;
//define sound velocity in cm/uS (cm pr. microsecond)
#define SOUND_VELOCITY 0.034
long duration;
float distanceCm;

bool SendFillLevel = false;
// End setup for Sensors


void InitDistanceSensor() {
  // wite stuff to initialize the sensor here
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

float GetDistance() {
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

  return distanceCm;
}

void ReadSensor2() {
  // wite stuff to initialize the sensor here


}

void ReadSensor3() {
  // wite stuff to initialize the sensor here


}
  //////////////////////////////////////
 ///////* Everything Sensor *//////////
//////////////////////////////////////





  ////////////////////////////////////////
 ///////* Everything Wireless *//////////
////////////////////////////////////////

#include <rn2xx3.h>
#include <HardwareSerial.h>

#define LoraRESET 21
#define LED_pin 13
#define RXD2 16
#define TXD2 17

HardwareSerial mySerial(1); // RX, TX !! labels on relay board is swapped !!

//create an instance of the rn2xx3 library,
//giving the software UART as stream to use,
//and using LoRa WAN
rn2xx3 myLora(mySerial);

#define addr "00D9DE45"
#define AppEUI "BE7A000000001465"
#define DevEUI "0004A30B00FE1D3F"
#define AppKey "B3C35A08F02D8D3988076D655BDD5B75"
#define DataRate 0

String LoraMessage = "";



void led_on()
{
  digitalWrite(LED_pin, 1);
}

void led_off()
{
  digitalWrite(LED_pin, 0);
}

void initialize_radio()
{
  //reset RN2xx3
  pinMode(LoraRESET, OUTPUT);
  digitalWrite(LoraRESET, LOW);
  delay(100);
  digitalWrite(LoraRESET, HIGH);

  delay(100); //wait for the RN2xx3's startup message
  mySerial.flush();

  //check communication with radio
  String hweui = myLora.hweui();
  Serial.println(hweui);
  while(hweui.length() != 16)
  {
    Serial.println("Communication with RN2xx3 unsuccessful. Power cycle the board.");
    Serial.println(hweui);
    delay(10000);
    hweui = myLora.hweui();
  }

  //print out the HWEUI so that we can register it via ttnctl
  Serial.println("When using OTAA, register this DevEUI: ");
  Serial.println(hweui);
  Serial.println("RN2xx3 firmware version:");
  Serial.println(myLora.sysver());
  //Serial.print("Setting data rate to ");
  //Serial.println(DataRate);
  //myLora.setDR(DataRate);

  //configure your keys and join the network
  Serial.println("Trying to join Cibicom");
  bool join_result = false;

  //ABP: initABP(String addr, String AppSKey, String NwkSKey);
  //join_result = myLora.initABP("02017201", "8D7FFEF938589D95AAD928C2E2E7E48F", "AE17E567AECC8787F749A62F5541D522");

  //OTAA: initOTAA(String AppEUI, String AppKey);
      /*
     * Initialise the RN2xx3 and join a network using over the air activation.
     *
     * AppEUI: Application EUI as a HEX string.
     *         Example "70B3D57ED00001A6"
     * AppKey: Application key as a HEX string.
     *         Example "A23C96EE13804963F8C2BD6285448198"
     * DevEUI: Device EUI as a HEX string.
     *         Example "0011223344556677"
     * If the DevEUI parameter is omitted, the Hardware EUI from module will be used
     * If no keys, or invalid length keys, are provided, no keys
     * will be configured. If the module is already configured with some keys
     * they will be used. Otherwise the join will fail and this function
     * will return false.
     */
  join_result = myLora.initOTAA(AppEUI, AppKey, DevEUI);

  while(!join_result){
    Serial.println("Unable to join. Are your keys correct, and do you have Cibicom coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined Cibicom");

}

void LoraSetup() {
    // LED pin is GPIO2 which is the ESP8266's built in LED
  pinMode(LED_pin, OUTPUT);
  led_on();

  mySerial.begin(57600);

  delay(1000); //wait for the arduino ide's serial console to open

  Serial.println("Startup");

  initialize_radio();

  //transmit a startup message
  myLora.tx("Markus's ESP32 er online");

  led_off();
}

void LoraTX(String TXmsg) {
  // wite stuff to initialize the wireless communication here
  led_on();
  Serial.println(myLora.getSNR());
  Serial.print("TXing: ");
  Serial.println(TXmsg);
  myLora.tx(TXmsg); //one byte, blocking function
  Serial.println(myLora.getRx()); 
  led_off();
}

  ////////////////////////////////////////
 ///////* Everything Wireless *//////////
////////////////////////////////////////






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


  /////////////////////////////////////////////
 ///////* Everything Låsemekanisme *//////////
/////////////////////////////////////////////








  ///////////////////////////////////
 ///////* The main code *///////////
///////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200); // Starts the serial communication

  //////////////////////////////////////////////////////////////////////////////////////////// EEPROM
  // Initialize the EEPROM library
  EEPROM.begin(EEPROM_SIZE);

  // Load the UID array from EEPROM
  LoadUIDs();

  //////////////////////////////////////////////////////////////////////////////////////////// DistanceSensor
  InitDistanceSensor();

  //////////////////////////////////////////////////////////////////////////////////////////// RFID
  SetupRFID();
  
  //////////////////////////////////////////////////////////////////////////////////////////// Wireless
  LoraSetup();

}

void loop() {

//////////////////////////////////////////////////////////////////////////////////////////// RFID
// Her skal låsen aktiveres, hvis kortet er rigtigt
if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) { 
  if (IsUIDKnown(mfrc522.uid.uidByte)){
    Serial.println("UID is known, give access");
  }
  else {
    Serial.println("UID is unknown");
  }
}

  
//////////////////////////////////////////////////////////////////////////////////////////// Lora
// herefter skal vi tjekke om der er beskeder på Lora
LoraMessage = myLora.getRx();
Serial.println(LoraMessage); 
  if (LoraMessage.startsWith("41")) {
  Serial.println("UID Now given access");
  StringToUID(LoraMessage);
  AddUID = true;
}
else if (LoraMessage.startsWith("52"))  {
  Serial.println("Removing UID: ");
  StringToUID(LoraMessage);
  RemoveUID = true;
}
else {
  Serial.println("Got something else...");
}

//////////////////////////////////////////////////////////////////////////////////////////// Fill Level
// Her finder vi afstanden med ultrasinic sensoren, og sender den
if (SendFillLevel) {
  Serial.println("Getting fill level");
  LoraTX(String(GetDistance()));
}




  //////////////////////////////////////////////////////////////////////////////////////////// EEPROM
  // Her gemmer vi vores KnownUIDs til EEPROM
  SaveUIDs(); 


}


