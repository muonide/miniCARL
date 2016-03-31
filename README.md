# miniCARL
miniature CARLbot software and design files for the Challenge Arkansas Robotics League

This tiny cousin of the original CARLbot has an Adafruit Feather 32u4 Bluefruit LE board as the main brains of the robot.Two DC motors are controlled by an Adafruit TB6612 motor driver breakout board. There is a miniature solderless breadboard available on the main board and each bot comes with a continuous rotation servo attached to pin #5.

miniCARL's are controlled via Bluetooth Low Energy and this Arduino sketch makes use of the Adafruit Bluefruit LE controller module to issue commands.
