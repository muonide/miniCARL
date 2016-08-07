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
const uint8_t PWMB = 10; //Speed control
const uint8_t BIN1 = 11; //Direction
const uint8_t BIN2 = 12; //Direction

////////////////////////////////////////
///// class and struct definitions /////
////////////////////////////////////////

// forward declaration of vector structs so that each can implicitly convert to the other
struct cart_vector;
struct cyl_vector;

/*
 * general purpose rectangular/Cartesian vector struct
 */
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

/*
 * general purpose cylindrical vector struct
 */
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

/*
 * a class containing a packet buffer and the length of the packet
 */
class BLE_packet {
    // the packet buffer
    uint8_t buffer[READ_BUFSIZE + 1];
    // the length of the packet
    uint8_t len;
  public:
    // constructor (flushes the buffer before use)
    BLE_packet(void) {
        this->flush();
    }
    // determine the packet's type
    uint8_t type(void) const {return this->buffer[1];}
    // return the length
    uint8_t length(void) const {return this->len;}
    // read from the buffer
    uint8_t read_buffer(uint8_t i) const {return buffer[i];}
    // receive a BLE packet
    bool get(Adafruit_BLE&, const int);
    // convert a buffer location to a double
    double to_double(const uint8_t index) const {
        // makes a double* from the address of buffer[i], then dereferences it
        return *const_cast<double*>(reinterpret_cast<const double*>(this->buffer+index));
    }
    // erase the buffer
    void flush(void) {
        this->len = 0;
        // zero out the buffer
        memset(this->buffer, 0, READ_BUFSIZE + 1);
    }
    // decay to bool when convenient --- zero-length ==> false (no packet)
    operator bool(void) const {return (this->len == 0 ? false : true);}
};

/*
 * button class
 */
class controller_button {
    // Is a button pressed?
    bool pressed;
    // which button was pressed (0 if none)
    uint8_t num;
  public:
    // constructor
    controller_button(void) {
        // start as a null button
        this->pressed = false;
        this->num = 0;
    }
    // return whether or not it's pressed
    bool is_pressed(void) const {return this->pressed;}
    bool number(void) const {return this->num;}
    // a function that reads the data from a packet
    void read_from_packet(const BLE_packet& packet) {
        if (packet.length() != 0 && packet.type() == 'B') {
            // convert "button number" char byte to int
            this->num = static_cast<uint8_t>(packet.read_buffer(2) - '0');
            // convert "pressed or released" char byte to bool
            this->pressed = static_cast<bool>(packet.read_buffer(3) - '0');
        }
        else {
            // If the length is zero or it's not a button packet, default to false and zero.
            this->pressed = false;
            this->num = 0;
        }
    }
};

/////////////////////////////////
///// Function declarations /////
/////////////////////////////////

// bluefruit functions
void initializeBluetooth(Adafruit_BluefruitLE_SPI&, const String);
//uint8_t readPacket(Adafruit_BLE*, uint16_t);
BLE_packet readPacket(Adafruit_BLE&, const int);
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
cart_vector vectorFromPacket(BLE_packet&);
void move(const cyl_vector&);
void stop(void);
// control functions
//bool getAccelerometer(cart_vector&);
//bool getButton(bool&, uint8_t&);

/////////////////////
///// templates /////
/////////////////////

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
