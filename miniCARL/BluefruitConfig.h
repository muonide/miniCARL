// COMMON SETTINGS
// ----------------------------------------------------------------------------------------------
// These settings are used in both SW UART, HW UART and SPI mode
// ----------------------------------------------------------------------------------------------
const uint8_t BUFSIZE = 128; // size of the read buffer for incoming data
const bool VERBOSE_MODE = true; // enables debug output if set to true
const int BLE_READPACKET_TIMEOUT = 500; // timeout in ms waiting to read a response


// HARDWARE UART SETTINGS
// ----------------------------------------------------------------------------------------------
// The following macros declare the HW serial port you are using. Uncomment
// this line if you are connecting the BLE to Leonardo/Micro or Flora
// ----------------------------------------------------------------------------------------------
#ifdef Serial1 // This makes it not complain on compilation if there's no Serial1.
  #define BLUEFRUIT_HWSERIAL_NAME Serial1
#endif


// SHARED UART SETTINGS
// ----------------------------------------------------------------------------------------------
// The following sets the optional Mode pin. It's recommended, but not required.
// ----------------------------------------------------------------------------------------------
const uint8_t BLUEFRUIT_UART_MODE_PIN = -1; // optional; -1 if unused
const uint8_t BLUEFRUIT_UART_RTS_PIN = -1; // optional; -1 if unused


// SHARED SPI SETTINGS
// ----------------------------------------------------------------------------------------------
// The following macros declare the pins to use for HW and SW SPI communication.
// SCK, MISO and MOSI should be connected to the HW SPI pins on the Uno when
// using HW SPI.  This should be used with nRF51822 based Bluefruit LE modules
// that use SPI (Bluefruit LE SPI Friend).
// ----------------------------------------------------------------------------------------------
const uint8_t BLUEFRUIT_SPI_CS = 8;
const uint8_t BLUEFRUIT_SPI_IRQ = 7;
const uint8_t BLUEFRUIT_SPI_RST = 4; // optional but recommended; -1 if unused
