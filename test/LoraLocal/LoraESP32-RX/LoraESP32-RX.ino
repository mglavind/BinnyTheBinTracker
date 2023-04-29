/*
 * Lora to Lora (P2P) using RN2484 and ESP8266
 * This sketch will listen for any lora messages sent with correct SP, frequency, and other settings (see code below). 
 * An LED will blink every time a package is received. The received payload is printed on serial monitor. 
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
 

#define RXD2 16
#define TXD2 17

String LastMessage;
String str;

void setup() {
  //output LED pin
  pinMode(13, OUTPUT);  // D7 on ESP8266
  led_off();
  
  // Open serial communications and wait for port to open:
  
  Serial.begin(115200);
  
  //Serial2.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  Serial2.setTimeout(1000);
  lora_autobaud();
  
  led_on();
  delay(1000);
  led_off();

  Serial.println("Initing LoRa");
  
  //Serial2.listen();
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  Serial2.println("sys get ver");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("mac pause");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
//  Serial2.println("radio set bt 0.5");
//  wait_for_ok();
  
  Serial2.println("radio set mod lora");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set freq 869100000");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set pwr 14");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set sf sf12");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set afcbw 41.7");
  str = Serial2.readStringUntil('\n');
  Serial.println(str);
  
  Serial2.println("radio set rxbw 20.8");  // Receiver bandwidth can be adjusted here. Lower BW equals better link budget / SNR (less noise). 
  str = Serial2.readStringUntil('\n');   // However, the system becomes more sensitive to frequency drift (due to temp) and PPM crystal inaccuracy. 
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
  
  Serial2.println("radio set cr 4/5"); // Maximum reliability is 4/8 ~ overhead ratio of 2.0
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
}

void loop() {
  Serial.println("waiting for a message");
  Serial2.println("radio rx 0"); //wait for 60 seconds to receive
  str = Serial2.readStringUntil('\n');

  Serial.println(str);
  delay(20);
  if ( str.indexOf("ok") == 0 )
  {
    str = String("");
    while(str=="")
    {
      str = Serial2.readStringUntil('\n');
    }
    if ( str.indexOf("radio_rx") == 0 )  //checking if data was reeived (equals radio_rx = <data>). indexOf returns position of "radio_rx"
    {
      toggle_led();
      Serial.println(str); //printing received data   
      String byteString = "0A1B2C3D";

      if (str.length()>9){
        LastMessage = str; 
      }
    }
    else
    {
      Serial.println("Received nothing");
    }
  }
  else
  {
    Serial.println("radio not going into receive mode");
    Serial.print("Last message: ");
    Serial.println(LastMessage);
    delay(1000);
  }
}

void lora_autobaud()
{
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

void toggle_led()
{
  digitalWrite(13, 1);
  delay(100);
  digitalWrite(13, 0);
}

void led_on()
{
  digitalWrite(13, 1);
}

void led_off()
{
  digitalWrite(13, 0);
}
