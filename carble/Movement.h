#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "PinMap.h"

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

// Function Prototypes
void move(int motor, int speed, int direction);
void stop();

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);

// the packet buffer
extern uint8_t packetbuffer[];

// Function Definitions

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



#endif
