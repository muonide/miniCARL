#include "miniCARL.h"

// Initialize Bluefruit SPI
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// buffers
uint8_t packetbuffer[READ_BUFSIZE+1]; // buffer to hold incoming characters

////////////////////////////////////////////////////////////////
////////////////// Function Definitions ////////////////////////
////////////////////////////////////////////////////////////////

/*
 * moves a motor given a cylindrical vector
 * @param cyl         a cylindrical vector
 *
 * @return            this function returns no value
 */
void move(const cyl_vector& cyl) {
  Serial << F("void move(const cyl_vector&) called!\n");
  double min_frac = 0.1; // fraction of vector length which maps to zero
  
  // The speed depends on the magnitude of the z component as compared to the vector length, converted to a uint8_t
  uint8_t speed = (cyl.z >= min_frac * cyl.length() ? (abs(cyl.z) / cyl.length() * 255) : 0);
  
  // the differential is the multipier that makes the wheels spin at different speeds at different input angles
  // 1 if theta <= 0, else --> 0 as theta --> +pi/2, and 0 for theta >= +pi/2
  double A_diff = (cyl.theta <= 0 ? 1 : (cyl.theta >= pi/2 ? 0 : (pi - cyl.theta) / pi));
  // 1 if theta >= 0, else --> 0 as theta --> -pi/2, and 0 for theta <= -pi/2
  double B_diff = (cyl.theta >= 0 ? 1 : (cyl.theta <= -pi/2 ? 0 : (pi + cyl.theta) / pi));
  
  // if the z coordinate is negative, reverse the direction
  //motor A
  digitalWrite(AIN1, (cyl.z < 0 ? HIGH : LOW));
  digitalWrite(AIN2, (cyl.z < 0 ? LOW : HIGH));
  // motor B
  digitalWrite(BIN1, (cyl.z < 0 ? HIGH : LOW));
  digitalWrite(BIN2, (cyl.z < 0 ? LOW : HIGH));
  
  // use the differential to set the PWM speeds
  analogWrite(PWMA, static_cast<uint8_t>(speed * A_diff));
  analogWrite(PWMB, static_cast<uint8_t>(speed * B_diff));
}

/*
 * Stops both motors
 * @param             this function takes no arguments
 *
 * @return            this function returns no value
 */
void stop(void) {
  analogWrite(PWMA, 0);
  analogWrite(PWMB, 0);
}

/*
 * Recieves an accelerometer packet from Bluefruit controller and returns a cartesian
 * vector by reference. Returns false when readPacket times out during one of the
 * samples.
 * @param vect        the (static) cartesian vector to be written to
 *
 * @return            whether or not a packet was received
 */
bool getAccelerometer(cart_vector& cart) {
  Serial << F("bool getAccelerometer(cart_vector&) called!\n");
  bool packetReceived = true;
  const uint8_t samples = 3; // the number of times to loop
  const uint8_t interval = 10; // sample delay interval
  const uint8_t weight = 3; // weighting of new samples

  for (int i = 0; i < samples && packetReceived; i++) {
    // Wait for new data to arrive
    uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  
    //Commands recieved from bluetooth accelerometer
    if(len != 0) {
      // If accelerometer packet recieved
      if(packetbuffer[1] == 'A') {
        cart.x += weight*parsefloat(packetbuffer+2);
        cart.y += weight*parsefloat(packetbuffer+6);
        cart.z += weight*parsefloat(packetbuffer+10);
      }
    }
    else {
        packetReceived = false;
    }
    delay(interval);
  }
  
  if (packetReceived) {
    cart.x /= weight*samples+1;
    cart.y /= weight*samples+1;
    cart.z /= weight*samples+1;
  }
  
  return packetReceived;
}

/*
 * Recieves a button packet from Bluefruit controller and returns information by
 * reference.
 * @param pressed     whether or not a button is pressed
 * @param button      which button is pressed (if any)
 *
 * @return            whether or not a packet was received
 */
bool getButton(bool& pressed, uint8_t& button) {
  Serial << F("bool getButton(bool&, uint8_t&) called!\n");
  bool packetReceived = false;

  // Wait for new data to arrive
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);

  //Commands recieved from bluetooth buttons
  if(len != 0) {
    if(packetbuffer[1] == 'B') { // If button packet recieved
      pressed = static_cast<bool>(packetbuffer[3] - '0'); // Convert "pressed or released" char byte to bool
      button = static_cast<uint8_t>(packetbuffer[2] - '0'); // Convert "button number" char byte to int
      packetReceived = true;
    }
  }

  return packetReceived;
}

/*
 * Initializes the Feather's bluetooth and waits for connection.
 * @param             this function takes no arguments
 *
 * @return            this function returns no value
 */
void initializeBluetooth(String name) {
  Serial.begin(9600);
  while(!Serial);

  if ( !ble.begin(VERBOSE_MODE) ) {
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
  String BROADCAST_CMD = String("AT+GAPDEVNAME=" + name);
  char buf[BROADCAST_CMD.length()+1];  // used to contain the broadcast name command
  BROADCAST_CMD.toCharArray(buf, BROADCAST_CMD.length()+1);

  //Change the broadcast device name here!
  if(ble.sendCommandCheckOK(buf)) {
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

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode\n\n"));

  // enable debug mode
  ble.verbose(true);

  // Wait for connection
  while (! ble.isConnected()) {
      delay(100);
  }

  // Set Bluefruit to DATA mode
  Serial.println(F("*****************"));
  Serial.println(F("Switching to DATA mode!"));
  Serial.println(F("*****************"));
  ble.setMode(BLUEFRUIT_MODE_DATA);
  
  // close the serial port
  //Serial.end();
}

/*
 * Crash in the event of a failure
 * @param err         the error message to repeat every 10s 
 * @return            this function returns no value
 */
void error(const __FlashStringHelper* err) {
  while(true) {
    Serial.println(err);
    delay(10000);
  }
}

/*
 * casts the four bytes at the specified address to a double
 * (i.e. makes *reinterpret_cast<double*>(uint8_t*) look prettier)
 * @param buffer      the specified address
 *
 * @return            the data at that address expressed as a double
 */
double parsefloat(uint8_t* buffer) {
  return *reinterpret_cast<double*>(buffer); // makes a double* from a uint8_t*, then dereferenes it
}

/*
 * waits for incoming data and parses it
 * @param ble         pointer to Adafruit BLE data array
 * @param timeout     how long to wait before timing out
 *
 * @return            0 if something went wrong; reply index (?) if successful
*/
uint8_t readPacket(Adafruit_BLE* ble, uint16_t timeout) {
  uint16_t origtimeout = timeout, replyidx = 0;

  memset(packetbuffer, 0, READ_BUFSIZE);

  while (timeout--) {
    if (replyidx >= 20)
      break;
    if ((packetbuffer[1] == 'A') && (replyidx == PACKET_ACC_LEN))
      break;
    if ((packetbuffer[1] == 'G') && (replyidx == PACKET_GYRO_LEN))
      break;
    if ((packetbuffer[1] == 'M') && (replyidx == PACKET_MAG_LEN))
      break;
    if ((packetbuffer[1] == 'Q') && (replyidx == PACKET_QUAT_LEN))
      break;
    if ((packetbuffer[1] == 'B') && (replyidx == PACKET_BUTTON_LEN))
      break;
    if ((packetbuffer[1] == 'C') && (replyidx == PACKET_COLOR_LEN))
      break;
    if ((packetbuffer[1] == 'L') && (replyidx == PACKET_LOCATION_LEN))
      break;

    while (ble->available()) {
      char c =  ble->read();
      if (c == '!') {
        replyidx = 0;
      }
      packetbuffer[replyidx] = c;
      replyidx++;
      timeout = origtimeout;
    }
    
    if (timeout == 0) break;
    delay(1);
  }

  packetbuffer[replyidx] = 0;  // null term

  if (!replyidx)  // no data or timeout 
    return 0;
  if (packetbuffer[0] != '!')  // doesn't start with '!' packet beginning
    return 0;
  
  // check checksum!
  uint8_t xsum = 0;
  uint8_t checksum = packetbuffer[replyidx-1];
  
  for (uint8_t i=0; i<replyidx-1; i++) {
    xsum += packetbuffer[i];
  }
  xsum = ~xsum;

  // Throw an error message if the checksum's don't match
  if (xsum != checksum) {
    Serial.print("Checksum mismatch in packet : ");
    printHex(packetbuffer, replyidx+1);
    return 0;
  }
  
  // checksum passed!
  return replyidx;
}

/*
 * prints a hexadecimal value in plain characters
 * @param  data       pointer to the byte data
 * @param  numBytes   data length in bytes
 *
 * @return            this function returns no value
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
