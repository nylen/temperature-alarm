#include <SoftwareSerial.h>
#include "RTClib/RTClib.cpp"
#include "Adafruit_FRAM_SPI/Adafruit_FRAM_SPI.cpp"
#include "Adafruit_NeoPixel/Adafruit_NeoPixel.cpp"

// ARDUINO PINS USED BY THIS PROJECT
// =================================
// LCD (serial 2400 baud): D3
// Temp sensor (ADC): A7 (signal), D2 (power)
// Clock (i2c): A5 (SCL), A4 (SDA)
// RAM (SPI): D12 (SCK), D11 (MISO), D10 (MOSI), D9 (CS)
// LED strip (WS2812): D7
// Button: D4
// Alarm (buzzer): D5
// LED (built-in): D13

#define PIN_LCD_INPUT  255 // none
#define PIN_LCD_OUTPUT 3
#define SS_INVERTED 1
SoftwareSerial bpi(PIN_LCD_INPUT, PIN_LCD_OUTPUT, SS_INVERTED);

#define PIN_TMP_POWER 2
#define PIN_TMP_DATA  A7

RTC_DS3231 rtc;

#define PIN_FRAM_SCK  12
#define PIN_FRAM_MISO 11
#define PIN_FRAM_MOSI 10
#define PIN_FRAM_CS    9
Adafruit_FRAM_SPI fram(PIN_FRAM_SCK, PIN_FRAM_MISO, PIN_FRAM_MOSI, PIN_FRAM_CS);

#define PIN_LEDS 7
#define NUM_LEDS 21
Adafruit_NeoPixel strip(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);

#define PIN_BUTTON 4
#define PIN_ALARM 5
#define PIN_LED_BUILTIN 13

// Utility functions
#include "lcd.h"
#include "temperature.h"
#include "memory.h"
#include "leds.h"
#include "button.h"
#include "time_step.h"
#include "alarm.h"

// Global variables
DateTime dtStart;
DateTime dtCurrent;
TimeSpan tsPowerOff;
uint8_t tempMin;
uint8_t tempMax;
uint8_t tempCurrent;
uint8_t alarmTemp;

void save_alarm_data() {
	mem_write_start_time(&dtStart);
	mem_write_latest_time(&dtCurrent);
	mem_write_time_power_off(&tsPowerOff);
	mem_write_min_max_temp(tempMin, tempMax);
}

void alarm_reset() {
	dtStart = dtCurrent = rtc.now();
	tsPowerOff = TimeSpan(0);
	tempMin = tempMax = tempCurrent = read_temp_uint8();
	alarm_clear();
	save_alarm_data();
}

void setup() {
	bool ok = true;

	digitalWrite(PIN_TMP_POWER, LOW);
	pinMode(PIN_TMP_POWER, OUTPUT);

	digitalWrite(PIN_LCD_OUTPUT, LOW);
	pinMode(PIN_LCD_OUTPUT, OUTPUT);

	digitalWrite(PIN_LEDS, LOW);
	pinMode(PIN_LEDS, OUTPUT);

	pinMode(PIN_BUTTON, INPUT_PULLUP);

	digitalWrite(PIN_LED_BUILTIN, LOW);
	pinMode(PIN_LED_BUILTIN, OUTPUT);

	digitalWrite(PIN_ALARM, LOW);
	pinMode(PIN_ALARM, OUTPUT);

	bpi.begin(2400);
	delay(10);

	Serial.begin(115200);

	bpi.write(bpi_clear, sizeof(bpi_clear));

	if (!rtc.begin()) {
		ok = false;
		LCD_COMMAND(bpi_line1);
		lcd_write_string(F("Clock error"));
	}

	if (!fram.begin()) {
		ok = false;
		LCD_COMMAND(bpi_line2);
		lcd_write_string(F("Memory error"));
	}

	if (!ok) {
		lcd_flush_bytes(bpi, LCD_BUFFER_SIZE);
		// do not execute loop(): https://stackoverflow.com/a/27832896/106302
		exit(0);
	}

	tempCurrent = read_temp_uint8();
	mem_read_min_max_temp(&tempMin, &tempMax);
	tempMin = min(tempMin, tempCurrent);
	tempMax = max(tempMax, tempCurrent);
	alarmTemp = mem_read_alarm_temp();

	if (rtc.lostPower()) {
		// Set date/time to when this program was compiled
		dtStart = dtCurrent = DateTime(F(__DATE__), F(__TIME__));
		rtc.adjust(dtStart);
		alarm_reset();
		// Display a message indicating that clock was reset
		LCD_COMMAND(bpi_line1);
		lcd_write_string(F("Check clock batt"));
		LCD_COMMAND(bpi_line2);
		lcd_write_string(F("Press button"));
		lcd_flush_bytes(bpi, LCD_BUFFER_SIZE);
		// Wait for button press and release
		while (!is_button_pressed());
		while (is_button_pressed());
	} else {
		// Read saved date/time values
		mem_read_start_time(&dtStart);
		DateTime dtLastOn;
		mem_read_latest_time(&dtLastOn);
		TimeSpan tsPowerOffPrev;
		mem_read_time_power_off(&tsPowerOffPrev);
		dtCurrent = rtc.now();
		tsPowerOff = dtCurrent - dtLastOn;
		// Equation 1:
		//   dtStart
		//   + (time previously online)
		//   + tsPowerOff (time previously offline)
		//   = dtLastOn
		// Equation 2:
		//   dtLastOn
		//   + (new time offline)
		//   = dtCurrent
		// Check for error conditions that make the equations unsatisfiable.
		// Any power-off times longer than 90 days will also cause a reset.
		if (
			dtLastOn.isBefore(dtStart) ||
			dtLastOn.isBefore(dtStart + tsPowerOffPrev) ||
			tsPowerOffPrev.totalseconds() / 3600 / 24 > 90 ||
			dtCurrent.isBefore(dtLastOn) ||
			tsPowerOff.totalseconds() / 3600 / 24 > 90 ||
			tempMax < tempMin
		) {
			lcd_set_temporary_message(F("Saved data reset"));
			alarm_reset();
		} else {
			// Power losses of less than 5 minutes will not cause an alarm
			if (tsPowerOff.totalseconds() >= 5/* * 60 */) {
				alarm_set();
				tsPowerOff = tsPowerOff + tsPowerOffPrev;
			} else if (tsPowerOffPrev.totalseconds()) {
				// Power was previously lost and alarm has not been reset
				alarm_set();
				tsPowerOff = tsPowerOffPrev;
			} else {
				// No time-based alarm
				tsPowerOff = tsPowerOffPrev;
			}
		}
	}

	Serial.println(millis());
	time_step();
}

void loop() {
	alarm_time_step();

	if (time_step_counter % STEPS_TEMP_READING == 0) {
		tempCurrent = read_temp_uint8();
		tempMin = min(tempMin, tempCurrent);
		tempMax = max(tempMax, tempCurrent);
		dtCurrent = rtc.now();

		if (!lcd_showing_temp_message()) {
			LCD_COMMAND(bpi_line1);
			if (
				(time_step_counter / STEPS_SWITCH_UP_DOWN) % 2 &&
				tsPowerOff.totalseconds()
			) {
				lcd_write_string(F("Off: "));
				lcd_write_TimeSpan(tsPowerOff);
			} else {
				lcd_write_string(F("On: "));
				lcd_write_TimeSpan(dtCurrent - dtStart - tsPowerOff);
			}
			lcd_write_16_spaces();
		}

		if (true/*TODO: !is_accepting_temp_input()*/) {
			LCD_COMMAND(bpi_line2);
			// Write min temperature
			lcd_write_char('L');
			lcd_write_integer(tempMin);
			lcd_write_char(223);
			lcd_write_char(' ');
			// Write current temperature
			lcd_write_char('=');
			lcd_write_integer(tempCurrent);
			lcd_write_char(223);
			lcd_write_char(' ');
			// Write min temperature
			lcd_write_char('H');
			lcd_write_integer(tempMax);
			lcd_write_char(223);
			lcd_write_4_spaces();
		}
	}

	if (time_step_counter % STEPS_SAVE_TIME_INFO == 0) {
		save_alarm_data();
	}

	if (alarm_active()) {
		led_step();
	} else if (alarm_was_active_last_time_step()) {
		led_clear_all();
	}

	digitalWrite(PIN_LED_BUILTIN, is_button_pressed() ? HIGH : LOW);
	digitalWrite(PIN_ALARM, is_button_pressed() ? HIGH : LOW);

	/* 8 bytes may take up to 16ms */
	lcd_flush_bytes(bpi, 8);

	time_step();
}
