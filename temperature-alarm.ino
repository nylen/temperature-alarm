#include <SoftwareSerial.h>

// LCD (serial 2400 baud): D3
// Temp sensor (ADC): A7 (signal), D2 (power)
// Clock (i2c): TODO
// RAM (SPI): TODO
// Button: D4

#define PIN_LED_BUILTIN 13

#define PIN_LCD_INPUT  255 // none
#define PIN_LCD_OUTPUT 3
#define SS_INVERTED 1
SoftwareSerial bpi(PIN_LCD_INPUT, PIN_LCD_OUTPUT, SS_INVERTED);

#define PIN_TMP_POWER 2
#define PIN_TMP_DATA  A7

#define PIN_BUTTON 4

#define MS_TIME_STEP 50

#define STEPS_TEMP_READING 20

#include "temperature.h"
#include "lcd.h"

unsigned long time_step_start = 0;
unsigned long time_step_max_elapsed = 0;
uint32_t time_step_counter = 0;
#define TIME_STEP_WRAP (((uint32_t)20)*3600)

void time_step(bool record_max = true) {
	time_step_counter = (time_step_counter + 1) % TIME_STEP_WRAP;
	unsigned long ms = millis();
	unsigned long elapsed = ms - time_step_start;
	if (record_max && elapsed > time_step_max_elapsed) {
		time_step_max_elapsed = elapsed;
	}
	if (elapsed >= MS_TIME_STEP) {
		Serial.println("!");
	} else if (elapsed > 0) {
		delay(MS_TIME_STEP - (ms % MS_TIME_STEP));
	} else {
		delay(MS_TIME_STEP);
	}
	time_step_start = millis();
}

void setup() {
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

	/* 8 bytes may take 16ms */
	lcd_flush_bytes(bpi, 8);

	time_step();
}
