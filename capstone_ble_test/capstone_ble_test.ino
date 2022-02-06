#include "ArduinoBLE.h"
BLEService testService("181C"); //names it "user data" in the GATT service

BLEUnsignedIntCharacteristic randomRead("2A69", BLERead | BLENotify);  //0x2A69 -> Position Data, notify enabled
BLEWordCharacteristic user_angle_range("2A57", BLERead | BLEWrite);     //0x2A57 -> Digital Output 
BLEBooleanCharacteristic start_flag("2BBB", BLERead | BLEWrite);                //0x2BBB -> Status Flag
  
const int led = 4;
long last_millis = 0;
bool local_start_flag = false;
int val = 0;

void setup() {
  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(led, OUTPUT);

  //init BLE
  if(!BLE.begin()){
    Serial.println("BLE init failed");
    while(1);
  }

  BLE.setLocalName("BLUE ARDUINO BOI");
  BLE.setAdvertisedService(testService);

  testService.addCharacteristic(user_angle_range);
  testService.addCharacteristic(randomRead);
  testService.addCharacteristic(start_flag);

  BLE.addService(testService);

  user_angle_range.writeValue(0);
  randomRead.writeValue(0);
  start_flag.writeValue(0); //off initially

  BLE.advertise();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  BLEDevice central = BLE.central(); // wait for a BLE central

  if (central) {  // if a central is connected to the peripheral
    Serial.print("Connected to central: ");

    Serial.println(central.address()); // print the central's BT address

    digitalWrite(LED_BUILTIN, HIGH); // turn on the LED to indicate the connection



    while (central.connected()) { // while the central is connected:     
      if(start_flag.written()){
        local_start_flag = start_flag.value();
        Serial.print("local start flag: "); Serial.println(local_start_flag);
        
      if(local_start_flag){
        Serial.println("angle updates started");
      }
      
      while(local_start_flag){
        val = analogRead(A1);
        randomRead.writeValue(val);
        Serial.print("value: "); 
        Serial.println(val);
      }//recording loop
      
      Serial.println("angle updates stopped");  //only happens after the angle update loop is done
      }
     
      
      }//connected while loop
    }

    digitalWrite(LED_BUILTIN, LOW); // when the central disconnects, turn off the LED
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());

}
