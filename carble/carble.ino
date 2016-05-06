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

#include <string.h>
#include <Arduino.h>
#include <SPI.h>
#include <Servo.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

String BROADCAST_NAME = "InsertBotNameHere";
 
String BROADCAST_CMD = String("AT+GAPDEVNAME=" + BROADCAST_NAME);

Servo myServo;

//Motor A
int PWMA = 5; //Speed control 
int AIN1 = 9; //Direction
int AIN2 = 6; //Direction

//Motor B
int PWMB = 10; //Speed control
int BIN1 = 11; //Direction
int BIN2 = 12; //Direction

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);


// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
//void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];

char buf[60];

void setup(void)
{
//  while (!Serial);  // required for Flora & Micro
//  delay(500);

  pinMode(A0, OUTPUT);  //IR LED pin set to OUTPUT
  pinMode(A1, INPUT);   //IR sensor pin set to INPUT
  pinMode(A2, OUTPUT);  //indicator buzzer on Analog Pin 2

  myServo.attach(13);
  
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);

  pinMode(PWMB, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  Serial.begin(115200);
  Serial.println(F("Adafruit Bluefruit App Controller Example"));
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
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
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

void loop(void)
{
  /* Wait for new data to arrive */
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0) return;

  /* Got a packet! */
  // printHex(packetbuffer, len);

  // Buttons
  if (packetbuffer[1] == 'B') {
    uint8_t num = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    Serial.print ("Button "); Serial.print(num);
      
    if (num == 1 & pressed) {
      Serial.println(" pressed");
      digitalWrite(A0, HIGH);
      delay(100);
      digitalWrite(A0, LOW);
    } 
    if (num == 2 & pressed) {
      Serial.println(" pressed");
      //read signals
    } 
    if (num == 3 & pressed) {
      Serial.println(" pressed");
      myServo.write(80);
      delay(100);
      myServo.write(90);
    }
    if (num == 4 & pressed) {
      Serial.println("pressed");
      myServo.write(100);
      delay(100);
      myServo.write(90);
    }
    if (num == 5 & pressed) {
      Serial.println(" pressed");
      move(1, 200, 0);
      move(2, 200, 0);
      delay(400);
      stop();
    } 
    if (num == 6 & pressed) {
      Serial.println(" pressed");
      move(1, 200, 1);
      move(2, 200, 1);
      delay(400);
      stop();
    } 
    if (num == 7 & pressed) {
      Serial.println(" pressed");
      move(1, 200, 0);
      move(2, 200, 1);
      delay(270);
      stop();
    } 
    if (num == 8 & pressed) {
      Serial.println(" pressed");
      move(1, 200, 1);
      move(2, 200, 0);
      delay(270);
      stop();
    } else {
      Serial.println(" released");
    }
    
  }

}


void move(int motor, int speed, int direction){

//  digitalWrite(STBY, HIGH); //disable standby

  boolean inPin1 = LOW;
  boolean inPin2 = HIGH;

  if(direction == 1){
    inPin1 = HIGH;
    inPin2 = LOW;
  }
  if(motor == 1){
    digitalWrite(AIN1, inPin1);
    digitalWrite(AIN2, inPin2);
    analogWrite(PWMA, speed);
  }else{
    digitalWrite(BIN1, inPin1);
    digitalWrite(BIN2, inPin2);
    analogWrite(PWMB, speed);
  }
}

void stop(){
//enable standby  
//  digitalWrite(STBY, LOW); 
analogWrite(PWMA, 0);
analogWrite(PWMB, 0);
}

