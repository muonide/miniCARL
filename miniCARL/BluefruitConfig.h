// COMMON SETTINGS
// ----------------------------------------------------------------------------------------------
// These settings are used in both SW UART, HW UART and SPI mode
// ----------------------------------------------------------------------------------------------
const uint8_t BUFSIZE = 128; // size of the read buffer for incoming data
const bool VERBOSE_MODE = true; // enables debug output if set to true
const int BLE_READPACKET_TIMEOUT = 500; // timeout in ms waiting to read a response
const unsigned int INACTIVITY_TIMEOUT = 5000; // time to wait for packets before stopping

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
// The following constants declare the pins to use for HW and SW SPI communication.
// SCK, MISO and MOSI should be connected to the HW SPI pins on the Uno when
// using HW SPI. This should be used with nRF51822 based Bluefruit LE modules
// that use SPI (Bluefruit LE SPI Friend).
// ----------------------------------------------------------------------------------------------
const uint8_t BLUEFRUIT_SPI_CS = 8;
const uint8_t BLUEFRUIT_SPI_IRQ = 7;
const uint8_t BLUEFRUIT_SPI_RST = 4; // optional but recommended; -1 if unused

// BLE PACKET AND BUFFER LENGTHS
// ----------------------------------------------------------------------------------------------
// The following lengths are used to parse packets.
// ----------------------------------------------------------------------------------------------
const uint8_t PACKET_ACC_LEN = 15; // length of accelerometer packets
const uint8_t PACKET_GYRO_LEN = 15; // length of gyroscope packets
const uint8_t PACKET_MAG_LEN = 15; // length of magnetometer packets
const uint8_t PACKET_QUAT_LEN = 19; // length of quaternion packets
const uint8_t PACKET_BUTTON_LEN = 5; // length of controller button packets
const uint8_t PACKET_COLOR_LEN = 6; // length of color picker packets
const uint8_t PACKET_LOCATION_LEN = 15; // length of GPS location packets
const uint8_t READ_BUFSIZE = 20; // length of the read buffer for incoming packets
