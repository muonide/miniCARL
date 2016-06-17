/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

#include <Arduino.h>
#include <string.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "Movement.h"
#include "PinMap.h"
#include "BluefruitConfig.h"

String BROADCAST_NAME = "Flexo";

String BROADCAST_CMD = String("AT+GAPDEVNAME=" + BROADCAST_NAME);

//Servo myServo;

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);

// the packet buffer
extern uint8_t packetbuffer[];

char buf[60];

void setup(void)
{
  /* Declare PinModes */
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  initializeBoard();

}

void loop(void)
{
  bool pressed;
  int button;
  
  if(getButton(pressed, button)){
    Serial.print ("Button "); 
    Serial.print(button);
    if(pressed){                                // On button press
      Serial.println(" pressed");
      if(button == 1){                          // Button 1 pressed
        // Button 1 stuff
      }else if(button == 2){                    // Button 2 pressed
        // Button 2 stuff
      }else if(button == 3){                    // Button 3 pressed
        // Button 3 stuff
      }else if(button == 4){                    // Button 4 pressed
        // Button 4 stuff
      }else if(button == 5){                    // Move forward?
        move(1, 200, 0);
        move(2, 200, 0);
      }else if(button == 6){                    // Move backward?
        move(1, 200, 1);
        move(2, 200, 1);
      }else if(button == 7){                    // Turn right?
        move(1, 200, 0);
        move(2, 200, 1);
      }else if(button == 8){                    // Turn left?
        move(1, 200, 1);
        move(2, 200, 0);
      }
    }
    if(!pressed){                               // On button release
      Serial.println(" released");
      if(button >= 5){                          // if one of the arrow keys
        stop();
      } else {                                  // if 1-4
        // numbered button stuff
      }
    }
  }
}


void initializeBoard(void){
  Serial.begin(9600);
  Serial.println(F("MiniCARL Init"));
  Serial.println(F("-----------------------------------------"));
  
  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  /* Perform a factory reset to make sure everything is in a known state */
  Serial.println(F("Performing a factory reset: "));
  if (! ble.factoryReset() ){
       error(F("Couldn't factory reset"));
  }

//Convert the name change command to a char array
  BROADCAST_CMD.toCharArray(buf, 60);

  //Change the broadcast device name here!
  if(ble.sendCommandCheckOK(buf)){
    Serial.println("name changed");
  }
  delay(250);

  //reset to take effect
  if(ble.sendCommandCheckOK("ATZ")){
    Serial.println("resetting");
  }
  delay(250);

  //Confirm name change
  ble.sendCommandCheckOK("AT+GAPDEVNAME");

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("*****************"));

  // Set Bluefruit to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("*****************"));
}

bool getButton(bool& pressed, int& button){
  bool packetReceived = true;
  
  /* Wait for new data to arrive */
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);

  //Commands recieved from bluetooth buttons
  if(len != 0){
    if(packetbuffer[1] == 'B'){                   // If button packet recieved
      pressed = packetbuffer[3] - '0';    // Convert "pressed or released" char byte to bool
      button = packetbuffer[2] - '0';     // Convert "button number" char byte to int
      
    }
  }else{
      packetReceived = false;
  }
  
  return packetReceived;
}

