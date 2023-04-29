

/*
 * Author: JP Meijers
 * Date: 2016-10-20
 *
 * Transmit a one byte packet via TTN. This happens as fast as possible, while still keeping to
 * the 1% duty cycle rules enforced by the RN2483's built in LoRaWAN stack. Even though this is
 * allowed by the radio regulations of the 868MHz band, the fair use policy of TTN may prohibit this.
 *
 * CHECK THE RULES BEFORE USING THIS PROGRAM!
 *
 * CHANGE ADDRESS!
 * Change the device address, network (session) key, and app (session) key to the values
 * that are registered via the TTN dashboard.
 * The appropriate line is "myLora.initABP(XXX);" or "myLora.initOTAA(XXX);"
 * When using ABP, it is advised to enable "relax frame count".
 *
 * Connect the RN2xx3 as follows:
 * RN2xx3 -- ESP8266
 * Uart TX -- GPIO4
 * Uart RX -- GPIO5
 * Reset -- GPIO15
 * Vcc -- 3.3V
 * Gnd -- Gnd
 *
 */
#include <Arduino.h>
#include <rn2xx3.h>
#include <HardwareSerial.h>

#define RESET 21
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


void initialize_radio()
{
  //reset RN2xx3
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);
  delay(100);
  digitalWrite(RESET, HIGH);

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

  while(!join_result)
  {
    Serial.println("Unable to join. Are your keys correct, and do you have Cibicom coverage?");
    delay(60000); //delay a minute before retry
    join_result = myLora.init();
  }
  Serial.println("Successfully joined Cibicom");

}
void led_on()
{
  digitalWrite(LED_pin, 1);
}

void led_off()
{
  digitalWrite(LED_pin, 0);
}

// the setup routine runs once when you press reset:
void setup() {


  // Open serial communications and wait for port to open:
  Serial.begin(57600);

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

// the loop routine runs over and over again forever:
void loop() {
    led_on();

    Serial.println(myLora.getSNR());

    Serial.println("TXing");

    myLora.tx("a-b-c"); //one byte, blocking function
    Serial.println(myLora.getRx()); 
    led_off();
    delay(50000);
}

