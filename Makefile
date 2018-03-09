# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile
# Debian package arduino-mk

# Gikfun Nano
# https://www.amazon.com/dp/B00SGMEH7Gd

BOARD_TAG     = nano328
ARDUINO_PORT  = /dev/ttyUSB0
AVRDUDE_ARD_BAUDRATE = 57600
ARDUINO_DIR   = /usr/share/arduino/
USER_LIB_PATH = $(realpath ./libraries)
ARDUINO_LIBS  = SoftwareSerial Wire

include /usr/share/arduino/Arduino.mk
