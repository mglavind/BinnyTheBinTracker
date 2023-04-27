/*
 * Lora to Lora (P2P) using RN2483 and ESP8266
 * This sketch will transmit a lora message with payload "20" every 2 seconds. 
 * An LED will blink every time a package is sent. Payload and package ID is printed on serial monitor. 
 * 
 * Code based on JP Meijers code from 30 September 2016
 * MN Petersen, Aug 2020
 * 
 * CONNECTIONS:
 * RN2483 - ESP8266:
 * TX     - D5 
 * RX     - D6
 * 3V3    - 3V3
 * GND    - GND
 * 
 * D7 on ESP8266 is connected to anode (long leg) on LED. Insert resistor between cathode and GND on ESP8266.
 * 
 */

#ifdef ESP32
  #define RXD2 16
  #define TXD2 17
#else
  #include <SoftwareSerial.h>
  SoftwareSerial Serial2(16, 17, false, 128); // (rxPin (D5), txPin (D6), inverse_logic, buffer size);
#endif





//SoftwareSerial Serial2(18,19,false);
//SoftwareSerial Serial2(16, 17, false, 128); // (rxPin (D5), txPin (D6), inverse_logic, buffer size);
//SoftwareSerial Serial2(18,19,false);

String msg;
String str;
int packageID = 0;

void setup() {
  //output LED pin
  pinMode(13, OUTPUT);  // D7 on ESP8266
  led_off();
  
  // Open serial communications and wait for port to open:
  Serial.begin(9600);  // Serial communication to PC
  Serial.println("Setting up lora");
  
  //Serial2.begin(9600);  // Serial communication to RN2483
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.setTimeout(1000);
  lora_autobaud();
  
  led_on();
  delay(1000);
  led_off();

  Serial.println("Initing LoRa");
  
  //Serial2.listen();
  str = Serial2.readStringUntil('/n');
  Serial.println(str);
  Serial2.println("sys get ver");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("mac pause");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
//  Serial2.println("radio set bt 0.5");  // Uncomment if we want to use FSK 
//  wait_for_ok();
  
  Serial2.println("radio set mod lora"); // Comment if we want to use FSK 
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set freq 869100000");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set pwr 14");  //max power 14 dBm
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set sf sf12");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set afcbw 41.7");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set rxbw 125");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
//  Serial2.println("radio set bitrate 50000");
//  wait_for_ok();
  
//  Serial2.println("radio set fdev 25000");
//  wait_for_ok();
  
  Serial2.println("radio set prlen 8");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set crc on");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set iqi off");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set cr 4/5");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set wdt 60000"); //disable for continuous reception
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set sync 12");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set bw 125");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);

  Serial.println("starting loop");
}

void loop() {
  led_on();
  msg = "4112345678";
  Serial.print("packageID = ");
  Serial.println(packageID);
  Serial.print("Message:");
  Serial.println(msg);
  Serial2.print("radio tx ");
  Serial2.println(msg);
  //Serial2.println(packageID);
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  led_off();
  delay(1000*60*0.5);
  packageID = packageID + 1;
}

void lora_autobaud()
{
  Serial.println("Autobaud");
  String response = "";
  while (response=="")
  {
    delay(1000);
    Serial2.write((byte)0x00);
    Serial2.write(0x55);
    Serial2.println();
    Serial2.println("sys get ver");
    response = Serial2.readStringUntil('\n');
  }
  Serial.println("Autobaud end");
}

/*
 * This function blocks until the word "ok\n" is received on the UART,
 * or until a timeout of 3*5 seconds.
 */
int wait_for_ok()
{
  str = Serial2.readStringUntil('\n');
  if ( str.indexOf("ok") == 0 ) {
    return 1;
  }
  else return 0;
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}
