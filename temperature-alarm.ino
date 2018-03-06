#include <SoftwareSerial.h>
#include "Adafruit_FRAM_SPI/Adafruit_FRAM_SPI.cpp"

// LCD (serial 2400 baud): D3
// Temp sensor (ADC): A7 (signal), D2 (power)
// Clock (i2c): TODO
// RAM (SPI): D12 (SCK), D11 (MISO), D10 (MOSI), D9 (CS)
// Button: D4

#define PIN_LED_BUILTIN 13

#define PIN_LCD_INPUT  255 // none
#define PIN_LCD_OUTPUT 3
#define SS_INVERTED 1
SoftwareSerial bpi(PIN_LCD_INPUT, PIN_LCD_OUTPUT, SS_INVERTED);

#define PIN_TMP_POWER 2
#define PIN_TMP_DATA  A7

// TODO Clock chip init

#define PIN_FRAM_SCK  12
#define PIN_FRAM_MISO 11
#define PIN_FRAM_MOSI 10
#define PIN_FRAM_CS    9
Adafruit_FRAM_SPI fram(PIN_FRAM_SCK, PIN_FRAM_MISO, PIN_FRAM_MOSI, PIN_FRAM_CS);

#define PIN_BUTTON 4

// Utility functions
#include "temperature.h"
#include "lcd.h"
#include "time_step.h"

void setup() {
	bool ok = true;

	digitalWrite(PIN_LED_BUILTIN, LOW);
	pinMode(PIN_LED_BUILTIN, OUTPUT);

	digitalWrite(PIN_TMP_POWER, LOW);
	pinMode(PIN_TMP_POWER, OUTPUT);

    digitalWrite(PIN_LCD_OUTPUT, LOW);
    pinMode(PIN_LCD_OUTPUT, OUTPUT);

	pinMode(PIN_BUTTON, INPUT_PULLUP);

    bpi.begin(2400);
    delay(10);

	Serial.begin(115200);

    bpi.write(bpi_clear, sizeof(bpi_clear));

	if (!fram.begin()) {
		ok = false;
		LCD_COMMAND(bpi_line1);
		lcd_write_string("FRAM error");
	}

	if (!ok) {
		lcd_flush_bytes(bpi, LCD_BUFFER_SIZE);
		// do not execute loop(): https://stackoverflow.com/a/27832896/106302
		exit(0);
	}

	Serial.println(millis());
	time_step(false);
}

void loop() {
	if (time_step_counter % STEPS_TEMP_READING == 0) {
		int temp = lround(read_temp());
		LCD_COMMAND(bpi_line1);
		lcd_write_integer(millis() % 30000);
		lcd_write_char(' ');
		lcd_write_integer(temp);
		lcd_write_char(' ');
		lcd_write_char(' ');

		LCD_COMMAND(bpi_line2);
		lcd_write_integer(time_step_max_elapsed);
		lcd_write_char(' ');
		lcd_write_char(' ');
	}

	if (time_step_counter % STEPS_TEMP_READING == STEPS_TEMP_READING / 2) {
		LCD_COMMAND(bpi_line1);
		lcd_write_string("test line 1 long string");
		LCD_COMMAND(bpi_line2);
		lcd_write_string("test line 2 long string");
	}

	/* 8 bytes may take up to 16ms */
	lcd_flush_bytes(bpi, 8);

	time_step();
}
