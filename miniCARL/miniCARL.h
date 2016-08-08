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
// Android or iOS
const bool IS_ANDROID = true;
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

/**
 * a class containing a packet buffer and the length of the packet
 */
class BLE_packet {
    // the packet buffer
    uint8_t buffer[READ_BUFSIZE + 1];
    // the length of the packet
    uint8_t len;
  public:
    /**
     * determine the packet's type
     * @return          the character corresponding to the packet's data type
     */
    uint8_t type(void) const {return this->buffer[1];}
    /**
     * return the packet length
     * @return          the length
     */
    uint8_t length(void) const {return this->len;}
    /**
     * read a character from the buffer
     * @param i         the index in the buffer array
     * @return          the value at the index
     */
    uint8_t read_buffer(uint8_t i) const {
        return (i >= 0 && i <= this->len ? buffer[i] : 0);
        }
    /**
     * get a packet from the bluetooth receiver
     */
    bool get(Adafruit_BLE&, const int);
    /**
     * convert a buffer location to a double if it won't result in a segfault
     * @param index     the index of the buffer array to start at
     * @return          a floating-point value
     */
    double to_double(const uint8_t index) const {
        double val = 0.0;
        // check array bounds
        if (index <= this->len - sizeof(double) && index >= 0) {
            // makes a double* from the address of buffer[i], then dereferences it
            val = *const_cast<double*>(reinterpret_cast<const double*>(this->buffer+index));
        }
        return val;
    }
    /**
     * erase all packet data
     */
    void flush(void) {
        this->len = 0;
        // zero out the buffer
        memset(this->buffer, 0, READ_BUFSIZE + 1);
    }
    /**
     * decay to bool when convenient (zero-length ==> false (no packet))
     */
    operator bool(void) const {return (this->len == 0 ? false : true);}
    // constructor (flushes the buffer before use)
    BLE_packet(void) {
        this->flush();
    }
};

// forward declaration of vector structs so that each can implicitly convert to the other
class cart_vector;
class cyl_vector;

/**
 * general purpose rectangular/Cartesian vector struct (x, y, z)
 */
class cart_vector {
  public:
    double x, y, z;
    /**
     * allow implicit conversion to a cylindrical vector (defined below)
     */
    operator cyl_vector(void) const;
    /**
     * calculate the length on demand
     * @return          the length of the vector
     */
    double length(void) const {
        // cartesian distance formula
        return sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
    }
    void invert(void) {
        this->x = -this->x;
        this->y = -this->y;
        this->z = -this->z;
    }
    /*
     * get the data from a packet
     */
    void read_from_packet(const BLE_packet& packet);
};

/**
 * general purpose cylindrical vector struct (r, theta, z)
 */
class cyl_vector {
  public:
    double r, theta, z;
    /**
     * allow implicit conversion to a cartesian vector (defined below)
     */
    operator cart_vector(void) const;
    /**
     * calculate the length on demand
     * @return          the length of the vector
     */
    double length(void) const {
        // pythagorean theorem
        return sqrt(this->r * this->r + this->z * this->z);
    }
    void invert(void) {
        this->theta += pi;
        if (this->theta >= 2*pi) {
            this->theta -= pi;
        }
        this->z = -this->z;
    }
    /*
     * get the data from a packet
     */
    void read_from_packet(const BLE_packet& packet);
};

/**
 * member function that allows implicit conversion of cartesian vectors to cylindrical vectors
 */
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

/**
 * member function that allows implicit conversion of cylindrical vectors to cartesian vectors
 */
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

/**
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
    /**
     * determine whether or not a button is pressed
     * @return          whether or not the button is pressed
     */
    bool is_pressed(void) const {return this->pressed;}
    /**
     * retrieve the button number
     * @return          the button's number
     */
    uint8_t number(void) const {return this->num;}
    /**
     * acquire the data from a packet
     * @param packet    the packet to be read
     */
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
//BLE_packet readPacket(Adafruit_BLE&, const int);
//double parsefloat(uint8_t*);
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
//void move(const cyl_vector&);
void move(const cart_vector&);
void stop(void);
// control functions
//bool getAccelerometer(cart_vector&);
//bool getButton(bool&, uint8_t&);

/////////////////////
///// templates /////
/////////////////////

/**
 * template for C++ stream-like Serial output
 */
template <class Console, typename T>
Console& operator<<(Console& port, const T& data) {
    port.print(data);
    return port;
}
/**
 * stream-like insertion of cartesian vectors
 */
template <class Console>
Console& operator<<(Console& port, const cart_vector& vect) {
    port << F("<") << vect.x << F(", ") << vect.y << F(", ") << vect.z << F(">");
    return port;
}
/*
 * stream-like insertion of cylindrical vectors
 */
/*
template <class Console>
Console& operator<<(Console& port, const cyl_vector& vect) {
    port << F("(") << vect.r << F(", ") << vect.theta << F(", ") << vect.z << F(")");
    return port;
}
*/

#endif
