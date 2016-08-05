#ifndef MINICARL_H
  #define MINICARL_H

// Adafruit Libraries
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

// Arduino Libraries
#include <Arduino.h>
#include <string.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
  #include <SoftwareSerial.h>
#endif
// Adafruit Libraries
#include <Adafruit_BLE.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <Adafruit_BluefruitLE_UART.h>
// miniCARL files
#include "BluefruitConfig.h"
// other libraries
#include <math.h>

// pi
const double pi = 2*asin(1);

// Pins and global variables
// Motor A pins
const uint8_t PWMA = 5; //Speed control
const uint8_t AIN1 = 9; //Direction
const uint8_t AIN2 = 6; //Direction
//Motor B pins
const uint8_t PWMB = 12; //Speed control
const uint8_t BIN1 = 10; //Direction
const uint8_t BIN2 = 11; //Direction

// Bluefruit packet and buffer lengths
const uint8_t PACKET_ACC_LEN = 15; // length of accelerometer packets
const uint8_t PACKET_GYRO_LEN = 15; // length of gyroscope packets
const uint8_t PACKET_MAG_LEN = 15; // length of magnetometer packets
const uint8_t PACKET_QUAT_LEN = 19; // length of quaternion packets
const uint8_t PACKET_BUTTON_LEN = 5; // length of controller button packets
const uint8_t PACKET_COLOR_LEN = 6; // length of color picker packets
const uint8_t PACKET_LOCATION_LEN = 15; // length of GPS location packets
const uint8_t READ_BUFSIZE = 20; // length of the read buffer for incoming packets

//////////////////////////////
///// struct definitions /////
//////////////////////////////

// forward declaration of vector structs so that each can implicitly convert to the other
struct cart_vector;
struct cyl_vector;

// rectangular/Cartesian vector struct
struct cart_vector {
  // allow implicit conversion to a cylindrical vector (defined below)
  operator cyl_vector(void) const;
  // mutually orthogonal and linear
  double x, y, z;
  // easily acquire the length on demand
  double length(void) const {
    // cartesian distance formula
    return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
  }
};

// cylindrical vector struct
struct cyl_vector {
  // allow implicit conversion to a cartesian vector (defined below)
  operator cart_vector(void) const;
  // mutually orthogonal, but theta is angular
  double r, theta, z;
  // easily acquire the length on demand
  double length(void) const {
    // pythagorean theorem
    return sqrt(this->r * this->r + this->z * this->z);
  }
};

// member function that allows implicit conversion of cartesian vectors to cylindrical vectors
// "inline" keyword avoids compiler complaints about multiple definition
inline cart_vector::operator cyl_vector(void) const {
  cyl_vector cyl;
  // r == sqrt(x^2 + y^2) (distance formula)
  cyl.r = sqrt(this->x * this->x + this->y * this->y);
  // theta == arctan(y/x) + pi with (0,0) giving zero
  cyl.theta = (this->x != 0 && this->y != 0 ? atan2(this->y, this->x) : 0);
  // z == z (obviously)
  cyl.z = this->z;

  return cyl;
}

// member function that allows implicit conversion of cylindrical vectors to cartesian vectors
// "inline" keyword avoids compiler complaints about multiple definition
inline cyl_vector::operator cart_vector(void) const {
  cart_vector cart;
  // x == r cos(theta)
  cart.x = this->r * cos(this->theta);
  // y == r sin(theta)
  cart.y = this->r * sin(this->theta);
  // z == z
  cart.z = this->z;

  return cart;
}

/////////////////////////////////
///// Function declarations /////
/////////////////////////////////

// bluefruit functions
void initializeBluetooth(String);
uint8_t readPacket(Adafruit_BLE*, uint16_t);
double parsefloat(uint8_t*);
void error(const __FlashStringHelper*);
void printHex(const uint8_t*, const uint32_t);
// button functions
void functionOne(); // button one
void functionOneReleased();
void functionTwo(); // button two
void functionTwoReleased();
void functionThree(); // button three
void functionThreeReleased();
void functionFour(); // button four
void functionFourReleased();
// movement functions
void move(const cyl_vector&);
void stop(void);
// control functions
bool getAccelerometer(cart_vector&);
bool getButton(bool&, uint8_t&);
// template for C++ stream-like Serial output
template <class Console, typename T>
Console& operator<<(Console& port, const T& data) {
  port.print(data);
  return port;
}
// stream-like insertion of cartesian vectors
template <class Console>
Console& operator<<(Console& port, const cart_vector& vect) {
  port << F("<") << vect.x << F(", ") << vect.y << F(", ") << vect.z << F(">");
  return port;
}
// stream-like insertion of cylindrical vectors
/*
template <class Console>
Console& operator<<(Console& port, const cyl_vector& vect) {
  port << F("(") << vect.r << F(", ") << vect.theta << F(", ") << vect.z << F(")");
  return port;
}
*/

#endif
