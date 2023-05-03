#include <Arduino.h>

unsigned long previousMillis = 0;
unsigned long currentMillis = 0;

// long timeToSleep = 1000*60*2; // Timer interval in milliseconds
long timeToSend = 1000*30;
long timeToRecieve = 1000*30;

bool SendViaLora = false;
bool BeginRecieveing = false;


unsigned long previousLoraRXMillis = 0;
unsigned long previousLoraTXMillis = 0;
unsigned long previousSleepMillis = 0;
unsigned long previousLEDMillis = 0;









  //////////////////////////////////////
 ///////////* Deep sleep */////////////
//////////////////////////////////////



//   https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
// Lav evt en interrupt også
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  60*3        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

// bool GoToSleep = false;


/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
// void print_wakeup_reason(){
//   esp_sleep_wakeup_cause_t wakeup_reason;

//   wakeup_reason = esp_sleep_get_wakeup_cause();

//   switch(wakeup_reason)
//   {
//     case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
//     case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
//     case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
//     case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;
//     case ESP_SLEEP_WAKEUP_ULP : Serial.println("Wakeup caused by ULP program"); break;
//     default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
//   }
// }


// void SetupSleepMode() {
//     //Increment boot number and print it every reboot
//   ++bootCount;
//   Serial.println("Boot number: " + String(bootCount));

//   //Print the wakeup reason for ESP32
//   print_wakeup_reason();

//   /*
//   First we configure the wake up source
//   We set our ESP32 to wake up every 5 seconds
//   */
//   esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
//   Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
//   " Seconds");
// }

  //////////////////////////////////////
 ///////////* Deep sleep */////////////
//////////////////////////////////////










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
  Serial.println("Saving to eeprom");
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
  Serial.println("Reading from eeprom");
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

const int SS_PIN  =  15;
const int RST_PIN  = 32;

int SIZE_BUFFER   =  18;
int MAX_SIZE_BLOCK = 16;
const int greenPin =    27;
const int redPin   =    33;



bool RemoveUID = false;
bool AddUID = false;





MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

MFRC522::MIFARE_Key key;

byte uid[4] = {0x00, 0x00, 0x00, 0x00 };
long LEDOnTime = 1000*3;

void SetupRFID() {
  Serial.println("Setting Up RFID");
  SPI.begin();             // Initialize SPI bus
  mfrc522.PCD_Init();      // Initialize MFRC522
  pinMode(greenPin, OUTPUT);
  pinMode(redPin, OUTPUT);

  Serial.println("done");
}


void GreenLEDOn() {
  digitalWrite(greenPin, 1);
  previousLEDMillis = currentMillis;
}

void RedLEDOn() {
  digitalWrite(redPin, 1);
  previousLEDMillis = currentMillis;
}



// Function to print UID bytes in hexadecimal format
void printUID(byte* uid, byte uidSize) {
    for (byte i = 0; i < uidSize; i++) {
        if (uid[i] < 0x10) {
            Serial.print("0");  // Add leading zero if necessary
        }
        Serial.print(uid[i], HEX);
        Serial.print(" ");
    }
    Serial.println(" ");
}

void StringToUID(String hexstring) {
    hexstring.remove(0,2);
    byte byteArray[4];
    for (int i = 0; i < hexstring.length(); i += 2) {
      uid[i/2] = (byte) strtol(hexstring.substring(i, i+2).c_str(), NULL, 16);
    }
    printUID(uid, 4);
}




int findEmptyRow() {
  for (int i = 0; i < 200; i++) {
    bool emptyRow = true;
    for (int j = 0; j < 4; j++) {
      if (knownUIDs[i][j] != 0xFF) {
        emptyRow = false;
        break;
      }
    }
    if (emptyRow) {
      return i;
    }
  }
  return -1;  // No empty rows found
}

void addUID(byte uid[]) {
  LoadUIDs();
  int emptyRow = findEmptyRow();
  if (emptyRow >= 0) {
    for (int i = 0; i < 4; i++) {
      knownUIDs[emptyRow][i] = uid[i];
    }
    // Optional: print a message to confirm the UID was added
    Serial.println("UID added to row " + String(emptyRow));
  } else {
    // Array is full, cannot add more UIDs
    Serial.println("Error: array is full");
  }
  SaveUIDs();
}

void ClearUID(byte uid[]) {
  LoadUIDs();
  for (int i = 0; i < 200; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (knownUIDs[i][j] != uid[j]) {
        match = false;
        break;
      }
    }
    if (match) {
      // Found a match, clear the row to indicate it is empty
      for (int l = 0; l < 4; l++) {
        knownUIDs[i][l] = 0xFF;
      }
      // Optional: print a message to confirm the UID was cleared
      Serial.println("UID cleared from row " + String(i));
      return;
    }
  }
  // UID not found in array
  Serial.println("Error: UID not found");
  SaveUIDs();
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

bool LoraIsSetup = false;

HardwareSerial mySerial(1); // RX, TX !! labels on relay board is swapped !!

//create an instance of the rn2xx3 library,
//giving the software UART as stream to use,
//and using LoRa WAN
rn2xx3 myLora(mySerial);

#define addr "00D9DE45"
#define AppEUI "BE7A000000001553"
#define DevEUI "0004A30B00F212AF"
#define AppKey "8DCEC7252D9C96883D2D6BDBD695B23C"
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
    delay(1000); // Was 1000 but set to 1000 to make it more speedy
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
  LoraIsSetup = true;
}

void LoraTX(String TXmsg) {
  // wite stuff to initialize the wireless communication here
  led_on();
  RedLEDOn();
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
  delay(100);

  //////////////////////////////////////////////////////////////////////////////////////////// EEPROM
  // Initialize the EEPROM library
  Serial.println("Initiating EEPROM");
  EEPROM.begin(EEPROM_SIZE);

  // Load the UID array from EEPROM
  LoadUIDs();

  //////////////////////////////////////////////////////////////////////////////////////////// DistanceSensor
  Serial.println("Initiating distance sensor");
  InitDistanceSensor();

  //////////////////////////////////////////////////////////////////////////////////////////// RFID
  SetupRFID();

  digitalWrite(redPin, 0);
  digitalWrite(greenPin, 0);
  
  //////////////////////////////////////////////////////////////////////////////////////////// Wireless
  
  byte *thirdRow = knownUIDs[100];
  for (int i = 0; i < 4; i++) {
    Serial.println(thirdRow[i]);
  }
  //////////////////////////////////////////////////////////////////////////////////////////// DeepSleep
  // Serial.println("Initiating DeepSleep");
  // SetupSleepMode();

}

void loop() {
  currentMillis = millis();

  //////////////////////////////////////////////////////////////////////////////////////////// RFID
  // Her skal låsen aktiveres, hvis kortet er rigtigt
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) { 
    if (IsUIDKnown(mfrc522.uid.uidByte)){
      Serial.println("UID is known, give access");
      GreenLEDOn();
    }
    else {
      Serial.println("UID is unknown");
      RedLEDOn();
    }
  }


  //////////////////////////////////////////////////////////////////////////////////////////// Lora
  // herefter skal vi tjekke om der er beskeder på Lora

  if (BeginRecieveing) {
    if (!LoraIsSetup) {
      LoraSetup();
    }

    LoraMessage = myLora.getRx();
    Serial.println(LoraMessage); 
    if (LoraMessage.startsWith("41")) {
      Serial.println("UID Now given access");
      StringToUID(LoraMessage);
      addUID(uid);
      AddUID = true;
      
    }
    else if (LoraMessage.startsWith("52"))  {
      Serial.println("Removing UID: ");
      StringToUID(LoraMessage);
      ClearUID(uid);
      RemoveUID = true;
    }
    else if (LoraMessage.isEmpty())  {
      // Serial.println("Nothing recieved");
    }
    else {
      Serial.println("Got nothing");
    }


  }





  /*
  Mail til fiskomaten@gmail.com

  kasser til trækasse 30x30x30




  */


  //////////////////////////////////////////////////////////////////////////////////////////// Timers

  if (currentMillis - previousLoraRXMillis >= timeToRecieve) {
    SendViaLora = true; // This way, GoToSleep can be triggered other places too.
    Serial.println("I should check for lora messages");
    BeginRecieveing = true;
    previousLoraRXMillis = currentMillis;
  }



  if (currentMillis - previousLoraTXMillis >= timeToSend) {
    SendViaLora = true; // This way, GoToSleep can be triggered other places too.

    Serial.println("I should send something via lora");
    Serial.println("Getting fill level");

    for (byte i = 0; i < 9; i++) {
      distanceCm = + GetDistance();
    }
    distanceCm = distanceCm/10;
    LoraTX(String(distanceCm));
    SendFillLevel = false; // Turning it off again

    SendFillLevel = true;
    previousLoraTXMillis = currentMillis;
  }

  // if (currentMillis - previousSleepMillis >= timeToSleep) {
  //   GoToSleep = true; // This way, GoToSleep can be triggered other places too.
  //   Serial.println("I should go to sleep now");
  //   previousSleepMillis = currentMillis;
  // }
  delay(20);

  /*
  if (currentMillis - previousLEDMillis >= LEDOnTime) {
    digitalWrite(greenPin, 0);
    digitalWrite(redPin, 0);
}
*/




// if(GoToSleep) {
//   Serial.println("Going to sleep now");
//   delay(1000);
//   Serial.flush(); 
//   esp_deep_sleep_start();
//   Serial.println("This will never be printed"); 
// }

}