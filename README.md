# miniCARL
## miniature CARLbot software and design files for the Challenge Arkansas Robotics League

This tiny cousin of the original CARLbot has an Adafruit Feather 32u4 Bluefruit LE board as the main brains of the robot. Two DC motors are controlled by an Adafruit TB6612 motor driver breakout board. There is a miniature solderless breadboard available on the main board and each bot comes with a continuous rotation servo. miniCARLs are controlled via Bluetooth Low Energy and this Arduino sketch makes use of the Adafruit Bluefruit LE controller module to issue commands.

You can add new functionality or modify this program. For instance, you'll note in the function void loop(void) a series of if (packet.type() == 'char') and if (button.number() == number) statements that contain the motion control. You can adjust the speed in controller mode by changing the standard direction vectors in miniCARL.ino or by simply changing MAX_SPEED_LEVEL in miniCARL.h. If you do not issue any commands for five seconds, then the bot will stop moving automatically.

To program the miniCARL:

1. Download the miniCARL folder and load into the Arduino IDE. Make sure you replace ``InsertBotNameHere'' with the name of your bot; keep that name in quotes or it won't work.

2. Ensure that the motor pins and any accessories are correctly mapped in the program. Motor pins, etc. are set in miniCARL.h, and Bluefruit LE pins are set in BluefruitConfig.h.

3. Upload the sketch to the Feather.

To control the bot, you'll need to download the Adafruit Bluefruit Connect LE app for your smartphone ([Android](https://play.google.com/store/apps/details?id=com.adafruit.bluefruit.le.connect&hl=en) or [iOS](https://itunes.apple.com/us/app/adafruit-bluefruit-le-connect/id830125974?mt=8)) or use the [ABLE app](https://github.com/adafruit/adafruit-bluefruit-le-desktop/releases) on a laptop.

Information on the commands for the Control Pad are found [here](ttps://learn.adafruit.com/bluefruit-le-connect-for-ios/controller).
Keep buttons 5, 6, 7, & 8 reserved for motion.

