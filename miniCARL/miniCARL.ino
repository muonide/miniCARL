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
#include "Config.h"
#include "BluefruitConfig.h"

// Enter the bots name
String BOT_NAME = "InsertBotName";

// Initialize Bluefruit SPI
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// Function prototypes
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void initializeBluetooth(void);
bool getButton(bool& pressed, int& button);
bool getAccelerometer(float& bearing, float& turn);

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

  initializeBluetooth();                        // Setup bluetooth and wait for connection
}

void loop(void)
{
  // Control variables
  bool pressed;
  int button;
  float velocity;                                  // Negitive values for forward movement
  float turn;                                   // Negitive values for left

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
  }else if(getAccelerometer(velocity, turn)){
    // Determine bearing
    bool bearing;                      // Forward bearing is true
    if(velocity < 0){
      bearing = true;
      velocity = -velocity;
    }else{
      bearing = false;
    }

    if(turn < 0){
      turn = -turn;
    }
    // Map accelerometer velocity and turn for move()
    //velocity = map(velocity, DIR_LOW_i, DIR_HIGH_i, DIR_LOW_f, DIR_HIGH_f);
    //turn = map(turn, TURN_LOW_i, TURN_HIGH_i, TURN_LOW_f, TURN_HIGH_f);

    velocity = VEL_HIGH_f * velocity;
    turn = TURN_HIGH_f * turn;

    if(velocity > VEL_HIGH_f){
      velocity = VEL_HIGH_f;
    }
    if(turn > TURN_HIGH_f){
      turn = TURN_HIGH_f;
    }
  
    Serial.print("Bearing: " + String(bearing));
    Serial.print(" Velocity: " + String(velocity));
    Serial.println("  Turn: " + String(turn));
    
    if(bearing){
      if(turn >= 0){                              // Turn right or straight
        move(1, velocity, bearing);
        move(2, velocity - turn, bearing);
      }else{                                      // Turn left
        move(1, velocity - turn, bearing);
        move(2, velocity, bearing);
      }
    }else{
      if(turn >= 0){                              // Turn left or straight
        move(1, velocity - turn, bearing);
        move(2, velocity, bearing);
      }else{                                      // Turn right
        move(1, velocity, bearing);
        move(2, velocity - turn, bearing);
      }
    }
  }
}

/*
 *  Initializes the Feather's bluetooth and waits for connection.
 *
 * @returns Void
 */
void initializeBluetooth(void){
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
 * Recieves a button packet from Bluefruit controller and returns information by
 * reference.
 *
 * @returns Bool to determine if a packet was recieved.
 */
bool getButton(bool& pressed, int& button){
  bool packetReceived = false;

  /* Wait for new data to arrive */
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);

  //Commands recieved from bluetooth buttons
  if(len != 0){
    if(packetbuffer[1] == 'B'){           // If button packet recieved
      pressed = packetbuffer[3] - '0';    // Convert "pressed or released" char byte to bool
      button = packetbuffer[2] - '0';     // Convert "button number" char byte to int
      packetReceived = true;
    }
  }

  return packetReceived;
}

/*
 * Recieves an accelerometer packet from Bluefruit controller and returns velocity
 * and turn by reference.
 *
 * @returns Bool to determine if a packet was recieved.
 */
bool getAccelerometer(float& velocity, float& turn){
  bool packetReceived = true;
  int samples = 2;
  int interval = 25;
  float v_cutoff = 0.5; // accelerometer data are reported in m/s^2 on Android; probably also on iOS
  
  for(int i = 0; i < samples; i++) {
    /* Wait for new data to arrive */
    uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  
    //Commands recieved from bluetooth accelerometer
    if(len != 0){
      if(packetbuffer[1] == 'A'){                // If accelerometer packet recieved
        turn += parsefloat(packetbuffer+6);      // accelerometer Y bearing describes forward and backward movement
        velocity += parsefloat(packetbuffer+10); // accelerometer Z bearing describes right and left movement
      }
    }
    else{
        packetReceived = false;
    }
    // wait until reading the next packet
    delay(interval);
  }
  turn /= samples+1;
  velocity /= samples+1;
  
  // If the velocity is below the cutoff velocity, make it zero.
  if (velocity < v_cutoff) {
    velocity = 0;
  }
  
  return packetReceived;
}
