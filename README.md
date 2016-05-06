# miniCARL
miniature CARLbot software and design files for the Challenge Arkansas Robotics League

This tiny cousin of the original CARLbot has an Adafruit Feather 32u4 Bluefruit LE board as the main brains of the robot.Two DC motors are controlled by an Adafruit TB6612 motor driver breakout board. There is a miniature solderless breadboard available on the main board and each bot comes with a continuous rotation servo.

miniCARL's are controlled via Bluetooth Low Energy and this Arduino sketch makes use of the Adafruit Bluefruit LE controller module to issue commands.

Download the carble folder and load into the Arduino IDE. Make sure you replace the InsertBotNameHere with the name of your bot; keep that name in quotes or it won't work.

Ensure that the motor pins and any accessories are correctly mapped in the program.  

You can add new functionality or modify this program. For instance, you'll note in the loop() a series of if(num==# & pressed) statements that contain the motion control.  You can adjust the speed with the second arguement of the move() function, the direction with the third arguement, and control the elapsed time with the delay command.  If you do not issue a stop command, then the bot will continue to move...be careful there.

When you're ready, connect the Feather board to your computer and upload the code.  

To control the bot you'll need to download the Adafruit Bluefruit Connect LE app for your smartphone or use the ABLE app on a laptop. 

iOS:  https://itunes.apple.com/us/app/adafruit-bluefruit-le-connect/id830125974?mt=8
Andriod: https://play.google.com/store/apps/details?id=com.adafruit.bluefruit.le.connect&hl=en
laptop: https://github.com/adafruit/adafruit-bluefruit-le-desktop/releases

