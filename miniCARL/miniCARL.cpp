#include "miniCARL.h"

////////////////////////////////
///// Function Definitions /////
////////////////////////////////

/**
 * moves the motors given a cartesian vector
 * @param cart          a cartesian vector
 */
void move(const cart_vector& cart) {
    Serial << F("void move(const cyl_vector&) called!\n");
    double min_frac = 0.1; // fraction of vector length which maps to zero

    // The speed depends on the magnitude of the z component as compared to the vector length,
    // converted to a uint8_t (implicitly)
    uint8_t speed = (abs(cart.z) >= min_frac * cart.length() ? (abs(cart.z) / cart.length() * 255) : 0);

    // The differential is the multipier that makes the wheels spin at different speeds at
    // different input angles. (ignores x)
    // 1 if y <= 0, else --> 0 as y --> length/2, and 0 for y >= length/2
    double B_diff = (cart.y <= 0 ? 1 : (cart.y >= cart.length()/2 ? 0 : (cart.length() - cart.y) / cart.length()));
    // 1 if y >= 0, else --> 0 as y --> -length/2, and 0 for y <= -length/2
    double A_diff = (cart.y >= 0 ? 1 : (cart.y <= -cart.length()/2 ? 0 : (cart.length() + cart.y) / cart.length()));

    // if the z coordinate is negative, reverse the direction
    //motor A
    digitalWrite(AIN1, (cart.z < 0 ? LOW : HIGH));
    digitalWrite(AIN2, (cart.z < 0 ? HIGH : LOW));
    // motor B
    digitalWrite(BIN1, (cart.z < 0 ? LOW : HIGH));
    digitalWrite(BIN2, (cart.z < 0 ? HIGH : LOW));

    // use the differential to set the PWM speeds
    analogWrite(PWMA, static_cast<uint8_t>(speed * A_diff));
    analogWrite(PWMB, static_cast<uint8_t>(speed * B_diff));
}

/**
 * stops both motors
 */
void stop(void) {
    analogWrite(PWMA, 0);
    analogWrite(PWMB, 0);
}

/**
 * Initializes the Feather's bluetooth and waits for connection.
 * @param ble           Adafruit BLE object (by reference)
 * @param name          the broadcast name
 */
void initializeBluetooth(Adafruit_BluefruitLE_SPI& ble, const String name) {
    Serial.begin(9600);
    //while(!Serial); // NOTE: Comment this line when disconnected from serial!

    if ( !ble.begin(VERBOSE_MODE) ) {
        error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
    }

    Serial.println(F("MiniCARL Bluetooth Initialization"));
    Serial.println(F("-----------------------------------------"));

    // Initialise the module
    Serial.print(F("Initialising the Bluefruit LE module: "));

    // Perform a factory reset to make sure everything is in a known state
    Serial.println(F("Performing a factory reset: "));
    if (!ble.factoryReset()){
        error(F("Couldn't factory reset"));
    }

    //Convert the name change command to a char array
    String BROADCAST_CMD = String("AT+GAPDEVNAME=" + name);
    char buf[BROADCAST_CMD.length() + 1];    // used to contain the broadcast name command
    BROADCAST_CMD.toCharArray(buf, BROADCAST_CMD.length() + 1);

    //Change the broadcast device name here!
    if(ble.sendCommandCheckOK(buf)) {
        Serial.println(F("name changed"));
    }
    delay(250);

    //reset to take effect
    if(ble.sendCommandCheckOK("ATZ")){
        Serial.println(F("resetting"));
    }
    delay(250);

    //Confirm name change
    ble.sendCommandCheckOK("AT+GAPDEVNAME");

    // Disable command echo from Bluefruit
    ble.echo(false);

    Serial.println(F("Requesting Bluefruit info:"));
    // Print Bluefruit information
    ble.info();

    Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode\n\n"));

    // disable debug mode
    ble.verbose(false);

    // Wait for connection
    while (!ble.isConnected()) {
        delay(100);
    }

    // Set Bluefruit to DATA mode
    Serial << F("*****************\n")
           << F("Switching to DATA mode!\n")
           << F("*****************");
    ble.setMode(BLUEFRUIT_MODE_DATA);

    // close the serial port
    //Serial.end();
}

/**
 * Crash in the event of a failure
 * @param err           the error message to repeat every 10s
 */
void error(const __FlashStringHelper* err) {
    while (true) {
        Serial.println(err);
        delay(10000);
    }
}

/**
 * Waits for incoming data, checks it, and stores it
 * @param ble           Adafruit BLE object (by reference)
 * @param timeout       how long to wait before timing out
 *
 * @return              whether or not a packet was received
*/
bool BLE_packet::get(Adafruit_BLE& ble, const unsigned int timeout) {
    Serial << F("BLE_packet packet::get(Adafruit_BLE&, uint16_t) called!\n");
    bool received = false;
    unsigned long int call_time = millis();

    while(millis() - call_time <= timeout && !received) {
        while (ble.available()) {
            char c = ble.read();
            if (c == '!') {
                this->len = 0;
            }
            this->buffer[this->len] = c;
            this->len++;
            call_time = millis();
        }

        if (this->len >= 20)
            received = true;
        if ((this->buffer[1] == 'A') && (this->len == PACKET_ACC_LEN))
            received = true;
        if ((this->buffer[1] == 'G') && (this->len == PACKET_GYRO_LEN))
            received = true;
        if ((this->buffer[1] == 'M') && (this->len == PACKET_MAG_LEN))
            received = true;
        if ((this->buffer[1] == 'Q') && (this->len == PACKET_QUAT_LEN))
            received = true;
        if ((this->buffer[1] == 'B') && (this->len == PACKET_BUTTON_LEN))
            received = true;
        if ((this->buffer[1] == 'C') && (this->len == PACKET_COLOR_LEN))
            received = true;
        if ((this->buffer[1] == 'L') && (this->len == PACKET_LOCATION_LEN))
            received = true;

        delay(1);
    }

    this->buffer[this->len] = 0; // null term

    // If no packet was received before timeout or the packet doesn't start with a '!',
    // then flush the packet
    if (this->len == 0 || this->buffer[0] != '!') {
        this->flush();
    }

    if (this->len != 0) {
        // check checksum!
        uint8_t xsum = 0;
        uint8_t checksum = this->buffer[this->len - 1];
        for (uint8_t i = 0; i < this->len - 1; i++) {
            xsum += this->buffer[i];
        }
        xsum = ~xsum;
    
        // Throw an error message if the checksum's don't match
        if (xsum != checksum) {
            Serial.print("Checksum mismatch in packet : ");
            printHex(this->buffer, this->len+1);
            this->flush();
            received = false;
        }
    }

    return received;
}

/**
 * Extracts vector data from a packet.
 * @param packet        the packet object
 *
 * @return              the cartesian vector to return
 */
void cart_vector::read_from_packet(const BLE_packet& packet) {
    if (packet.type() == 'A') {
        // the data are in 4-byte doubles after the ! and type character
        this->x = packet.to_double(2);
        this->y = packet.to_double(6);
        this->z = packet.to_double(10);
    }
    // if the vector is an iOS vector, it needs to be inverted
    if (IS_ANDROID == false) {
        this->invert();
        this->x *= 9.8;
        this->y *= 9.8;
        this->z *= 9.8;
    }
}

/**
 * Extracts vector data from a packet.
 * @param packet        the packet object
 *
 * @return              the cylindrical vector to return
 */
void cyl_vector::read_from_packet(const BLE_packet& packet) {
    double x, y, z;
    if (packet.type() == 'A') {
        // the data are in 4-byte doubles after the ! and type character
        x = packet.to_double(2);
        y = packet.to_double(6);
        z = packet.to_double(10);
    }
    // if the vector is an iOS vector, it needs to be inverted
    if (IS_ANDROID == false) {
        x *= -9.8;
        y *= -9.8;
        z *= -9.8;
    }
    // convert and save
    this->r = sqrt(x * x + y * y);
    this->theta = (x != 0 && y != 0 ? atan2(y, x) : 0);
    this->z = z;
}

/**
 * prints a hexadecimal value in plain characters
 * @param  data         pointer to the byte data
 * @param  numBytes     data length in bytes
*/
void printHex(const uint8_t* data, const uint32_t numBytes) {
    uint32_t szPos;
    for (szPos = 0; szPos < numBytes; szPos++) {
        Serial.print(F("0x"));
        // Append leading 0 for small values
        if (data[szPos] <= 0xf) {
            Serial.print(F("0"));
            Serial.print(data[szPos] & 0xf, HEX);
        }
        else {
            Serial.print(data[szPos] & 0xff, HEX);
        }
        // Add a trailing space if appropriate
        if ((numBytes > 1) && (szPos != numBytes - 1)) {
            Serial.print(F(" "));
        }
    }
    Serial.println();
}

/**
 * moves the motors given a cylindrical vector
 * @param cyl           a cylindrical vector
 */
/*
void move(const cyl_vector& cyl) {
    Serial << F("void move(const cyl_vector&) called!\n");
    double min_frac = 0.1; // fraction of vector length which maps to zero

    // The speed depends on the magnitude of the z component as compared to the vector length,
    // converted to a uint8_t (implicitly)
    uint8_t speed = (abs(cyl.z) >= min_frac * cyl.length() ? (abs(cyl.z) / cyl.length() * 255) : 0);
    
    // The differential is the multipier that makes the wheels spin at different speeds at
    // different input angles.
    // 1 if theta <= 0, else --> 0 as theta --> +pi/2, and 0 for theta >= +pi/2
    double A_diff = (cyl.theta <= 0 ? 1 : (cyl.theta >= pi/2 ? 0 : (pi - cyl.theta) / pi));
    // 1 if theta >= 0, else --> 0 as theta --> -pi/2, and 0 for theta <= -pi/2
    double B_diff = (cyl.theta >= 0 ? 1 : (cyl.theta <= -pi/2 ? 0 : (pi + cyl.theta) / pi));

    // if the z coordinate is negative, reverse the direction
    //motor A
    digitalWrite(AIN2, (cyl.z < 0 ? HIGH : LOW));
    digitalWrite(AIN1, (cyl.z < 0 ? LOW : HIGH));
    // motor B
    digitalWrite(BIN2, (cyl.z < 0 ? HIGH : LOW));
    digitalWrite(BIN1, (cyl.z < 0 ? LOW : HIGH));

    // use the differential to set the PWM speeds
    analogWrite(PWMA, static_cast<uint8_t>(speed * A_diff));
    analogWrite(PWMB, static_cast<uint8_t>(speed * B_diff));
}
*/
