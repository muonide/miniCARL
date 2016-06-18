/*
 * MiniCARL Basic Platform
 *
 * @version 0.1.0
 *
 * This program provides the basic structuring for a miniCARL bot. This file
 * contains the scaffolding for the bot's movemement and controls. The robot's
 * hardware is detailed in the included files. MiniCARL bots have been
 * designed to be operated via bluetooth using Adafruit's Bluefruit controller
 * app. This file was adapted from Adafruit's Feather bluetooth example:
 *                              {{ URL HERE }}
 *
 * {{ LICENSE information }}
 */

// Arduino Libraries
#include <Arduino.h>
#include <string.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

// Adafruit Libraries
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

// MiniCARL Libraries
#include "miniCARL.h"
#include "PinConfig.h"
#include "BluefruitConfig.h"

// Enter the bots name
String BOT_NAME = "InsertBotName";

// Initialize Bluefruit SPI
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// Function prototypes
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);

// Define buffers
extern uint8_t packetbuffer[];
char buf[60];

void setup(void)
{
  /* Declare motor controller pin modes */
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  initializeBoard();                            // Setup bluetooth and wait for connection
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
      }else if(button == 5){                    // Move forward
        move(1, 200, 0);
        move(2, 200, 0);
      }else if(button == 6){                    // Move backward
        move(1, 200, 1);
        move(2, 200, 1);
      }else if(button == 7){                    // Turn right
        move(1, 200, 0);
        move(2, 200, 1);
      }else if(button == 8){                    // Turn left
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

/*
 *  Initializes the Feather's bluetooth and waits for connection.
 *
 * @returns Void
 */
void initializeBoard(void){
  Serial.begin(9600);

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }

  Serial.println(F("MiniCARL Bluetooth Initialization"));
  Serial.println(F("-----------------------------------------"));

  // Initialise the module
  Serial.print(F("Initialising the Bluefruit LE module: "));

  // Perform a factory reset to make sure everything is in a known state
  Serial.println(F("Performing a factory reset: "));
  if (! ble.factoryReset() ){
       error(F("Couldn't factory reset"));
  }

  //Convert the name change command to a char array
  String BROADCAST_CMD = String("AT+GAPDEVNAME=" + BOT_NAME);
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

  // Disable command echo from Bluefruit
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  // Print Bluefruit information
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println();

  // Disable debug mode
  ble.verbose(false);

  // Wait for connection
  while (! ble.isConnected()) {
      delay(500);
  }

  // Set Bluefruit to DATA mode
  Serial.println(F("*****************"));
  Serial.println( F("Switching to DATA mode!") );
  Serial.println(F("*****************"));
  ble.setMode(BLUEFRUIT_MODE_DATA);

  return;
}

/*
 * Recieves packet from Bluefruit controller and returns information by
 * reference.
 *
 * @returns Bool to determine if a packet was recieved.
 */
bool getButton(bool& pressed, int& button){
  bool packetReceived = true;

  /* Wait for new data to arrive */
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);

  //Commands recieved from bluetooth buttons
  if(len != 0){
    if(packetbuffer[1] == 'B'){           // If button packet recieved
      pressed = packetbuffer[3] - '0';    // Convert "pressed or released" char byte to bool
      button = packetbuffer[2] - '0';     // Convert "button number" char byte to int

    }
  }else{
      packetReceived = false;
  }

  return packetReceived;
}
