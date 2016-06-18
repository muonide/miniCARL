/*
 * MiniCARLConfiguration File
 */

#ifndef CONFIG_H
#define CONFIG_H

// Motor A pins
#define PWMA          5 //Speed control
#define AIN1          9 //Direction
#define AIN2          6 //Direction

//Motor B pins
#define PWMB          12 //Speed control
#define BIN1          10 //Direction
#define BIN2          11 //Direction

// Accelerometer maping constants
#define DIR_LOW_i     -9.9
#define DIR_HIGH_i    9.9
#define DIR_LOW_f     0
#define DIR_HIGH_f    200
#define TURN_LOW_i    -9.9
#define TURN_HIGH_i   9.9
#define TURN_LOW_f    0
#define TURN_HIGH_f   200

#endif
