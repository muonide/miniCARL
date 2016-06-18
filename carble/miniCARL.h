#ifndef MINICARL_H
#define MINICARL_H

// MiniCARL Libraries
#include "PinConfig.h"

// Adafruit Libraries
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

// Function Definitions

/*
 * Motor control function move a motor given the speed and direction.
 *
 * @returns Void
 */
void move(int motor, int speed, int direction){
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

  return;
}

/*
 * Stops both motors
 *
 * @returns Void
 */
void stop(){
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);

  return;
}

/*
 * An error handler for strings in flash memory.
 *
 * @returns Void
 */
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

#endif
