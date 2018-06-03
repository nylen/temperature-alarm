# Arduino Temperature Alarm

This is the code for a device that uses a temperature sensor to monitor a
freezer and trigger an alarm if the temperature goes above a programmable
threshold, or if the device has been without power for more than 5 minutes.

Temperature sensor:
https://www.atlas-scientific.com/product_pages/probes/env-tmp.html

Power-off detection using:

- Real-time clock chip: https://www.adafruit.com/product/3013
- Persistent memory: https://www.adafruit.com/product/1897

## Installing/Updating

Download and install the Arduino IDE software:
https://www.arduino.cc/en/Main/Software

Take apart the device and connect a USB cable between your computer and the
main board.  The device should be detected as a serial port and it should
appear in the Arduino IDE under `Tools > Port`.

Download this code from
https://github.com/nylen/temperature-alarm/archive/master.zip and unpack it.

Open the `temperature-alarm.ino` file in the Arduino IDE.

Make sure `Tools > Board` is set to Arduino Nano, and click the toolbar button
for `Upload` (the icon looks like a right-facing arrow).

## Using the temperature alarm

The device has a single button that controls all functions.

To **view the currently set alarm temperature**, press the button and release
it after less than one second.

To **clear an active alarm**, as well as the highest and lowest detected
temperatures and the amount of time that the device was without power, if any,
hold down the button for one second.

Holding the button down for one second will also enter **alarm temperature
input mode** and the display will show `Input: 0`.  Press and release the
button to change the first digit of the alarm temperature.

When the first digit is set correctly, hold down the button for one second.
The second digit will appear - press and release the button to change it.

When the second digit is set correctly, hold down the button for one second,
and the new alarm temperature will be saved.

If the device is in **alarm temperature input mode** and no input is received
for 3 seconds, the temperature input will be cancelled and the device will
return to normal operation.

To **enable or disable the alarm sound**, enter an alarm temperature of `00` as
described above.  This will not set a new alarm temperature (the alarm
temperature is programmable between 1 and 99 degrees Fahrenheit).

When the current alarm temperature is being shown (immediately after setting it
or after a short press of the button during normal operation), **and** the
alarm sound is disabled, a small `x` will be displayed in the top right corner
of the display along with the current alarm temperature.
