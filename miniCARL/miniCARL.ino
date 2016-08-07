/*
 * MiniCARL Basic Platform
 *
 * @version ????
 *
 * This program provides the basic structuring for a miniCARL bot. This file
 * contains the scaffolding for the bot's movemement and controls, while
 * miniCARL.h and miniCARL.cpp contain essential objects, functions, and
 * templates. The robot's hardware is detailed in the included files. MiniCARL
 * bots have been designed to be operated via bluetooth using Adafruit's
 * Bluefruit controller app. This file was adapted from Adafruit's Feather
 * bluetooth example:
 * {{ URL HERE }}
 *
 * {{ LICENSE INFO HERE }}
 */

#include "miniCARL.h"

// Initialize Bluefruit SPI
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

/**
 * standard direction vectors
 */
const cyl_vector forward{0, 0, 1};
const cyl_vector reverse{0, 0, -1};
const cyl_vector right{0, pi, 1};
const cyl_vector left{0, -pi, 1};

// Enter the bot's name
String BOT_NAME = "This one.";

// define button functions here
void functionOne() {} // button one
void functionOneReleased() {}
void functionTwo() {} // button two
void functionTwoReleased() {}
void functionThree() {} // button three
void functionThreeReleased() {}
void functionFour() {} // button four
void functionFourReleased() {}

void setup(void) {
    // Declare motor controller pin modes
    pinMode(PWMA, OUTPUT);
    pinMode(AIN1, OUTPUT);
    pinMode(AIN2, OUTPUT);
    pinMode(PWMB, OUTPUT);
    pinMode(BIN1, OUTPUT);
    pinMode(BIN2, OUTPUT);

    // Setup bluetooth and wait for connection
    initializeBluetooth(ble, BOT_NAME);
    // start Serial
    //Serial.begin(9600);
}

void loop(void) {
    BLE_packet packet;
    cart_vector vector{0,0,0};
    controller_button button;

    // if a packet was received . . .
    // note: packet implicitly converts to a bool
    if (packet.get(ble, BLE_READPACKET_TIMEOUT)) {
        // . . . check the packet type
        // accelerometer
        if (packet.type() == 'A') {
            // get a vector from the packet
            vector = vectorFromPacket(packet);
            // serial debugging
            Serial << F("accelerometer reading: ") << vector << F("\n");
            // move using the vector
            move(vector);
        }
        // button
        else if (packet.type() == 'B') {
            button.read_from_packet(packet);
            Serial << F("button ") << button.number();

            // On button press . . .
            if (button.is_pressed()) {
                Serial << F(" pressed\n");
                // Button 1 pressed
                if (button.number() == 1) {
                    functionOne();
                }
                // Button 2 pressed
                else if (button.number() == 2) {
                    functionTwo();
                }
                // Button 3 pressed
                else if (button.number() == 3) {
                    functionThree();
                }
                // Button 4 pressed
                else if (button.number() == 4) {
                    functionFour();
                }
                // Move forward
                else if (button.number() == 5) {
                    move(forward);
                }
                // Move backward
                else if (button.number() == 6) {
                    move(reverse);
                }
                // Turn right
                else if (button.number() == 7) {
                    move(right);
                }
                // Turn left
                else if (button.number() == 8) {
                    move(left);
                }
                else {
                    Serial << F("not implemented\n");
                }
            }
            // On button release (i.e. if the button is no longer pressed) . . .
            else {
                Serial << F(" released\n");
                if (button.number() == 1) {
                    functionOneReleased();
                }
                else if (button.number() == 2) {
                    functionTwoReleased();
                }
                else if (button.number() == 3) {
                    functionThreeReleased();
                }
                else if (button.number() == 4) {
                    functionFourReleased();
                }
                // stop the motors if one of the arrow keys was released
                else {
                    stop();
                }
            }
        }
        // other type
        else {
            Serial << F("No action taken.\n");
        }
    }
}

/*// (old void loop(void) preserved)
void loop(void) {
    Serial << F("void loop(void) called!\n");
    static cart_vector vector{0,0,0};
    static bool pressed; // whether or not the button is pressed
    static uint8_t button; // which button is pressed, if any

    if(getButton(pressed, button)) {
        Serial << F("button ") << button;

        // On button press . . .
        if(pressed) {
            Serial << F(" pressed\n");
            // Button 1 pressed
            if(button == 1) {
                functionOne();
            }
            // Button 2 pressed
            else if(button == 2) {
                functionTwo();
            }
            // Button 3 pressed
            else if(button == 3) {
                functionThree();
            }
            // Button 4 pressed
            else if(button == 4) {
                functionFour();
            }
            // Move forward
            else if(button == 5) {
                move(forward);
            }
            // Move backward
            else if(button == 6) {
                move(reverse);
            }
            // Turn right
            else if(button == 7) {
                move(right);
            }
            // Turn left
            else if(button == 8) {
                move(left);
            }
            else {
                Serial << F("not implemented\n");
            }
        }
        // On button release (i.e. if the button is no longer pressed) . . .
        else {
            Serial << F(" released\n");
            if (button == 1) {
                functionOneReleased();
            }
            else if (button == 2) {
                functionTwoReleased();
            }
            else if (button == 3) {
                functionThreeReleased();
            }
            else if (button == 4) {
                functionFourReleased();
            }
            // stop the motors if one of the arrow keys was released
            else {
                stop();
            }
        }
    }
    // otherwise, if the accelerometer is active . . .
    else if(getAccelerometer(vector)) {
        // serial debugging
        Serial << F("accelerometer reading: ") << vector << F("\n");
        move(vector);
    }
    else {
        Serial << F("No action taken\n");
    }
}
*/
