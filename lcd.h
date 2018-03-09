// from http://seetrontech.blogspot.com/2011/12/arduino-hello-world-for-bpi-216-serial.html
const byte bpi_line1[] = { 254,128 };
const byte bpi_line2[] = { 254,192 };
const byte bpi_clear[] = { 254,1 /* also send (254,128) if 9600 baud */ };

#define LCD_COMMAND(byte_array) (lcd_write_bytes(byte_array, sizeof(byte_array)))

#include "circular_buffer.h"

#define LCD_BUFFER_SIZE 64
byte lcd_buffer_[LCD_BUFFER_SIZE];
circular_buffer<unsigned char> lcd_buffer(lcd_buffer_, LCD_BUFFER_SIZE);

void lcd_write_string(const char *text) {
	size_t i = 0;
	while (text[i] && !lcd_buffer.full()) {
		lcd_buffer.put(text[i++]);
	}
}

void lcd_write_char(unsigned char c) {
	if (!lcd_buffer.full()) {
		lcd_buffer.put(c);
	}
}

void lcd_write_string(const __FlashStringHelper *ftext) {
	// https://arduino.stackexchange.com/a/4631
	const char PROGMEM *p = (const char PROGMEM *) ftext;
	unsigned char c;
	while (c = pgm_read_byte(p++)) {
		lcd_write_char(c);
	}
}

void lcd_write_bytes(const byte command[], size_t len) {
	size_t i = 0;
	while (i < len && !lcd_buffer.full()) {
		lcd_buffer.put(command[i++]);
	}
}

void lcd_write_integer(int16_t n) {
	bool needZeroes = false;
	if (n < 0) {
		lcd_write_char('-');
		n = -n;
	}
	if (n > 10000) {
		lcd_write_char('0' + (n / 10000));
		n %= 10000;
		needZeroes = true;
	}
	if (n > 1000 || needZeroes) {
		lcd_write_char('0' + (n / 1000));
		n %= 1000;
		needZeroes = true;
	}
	if (n > 100 || needZeroes) {
		lcd_write_char('0' + (n / 100));
		n %= 100;
		needZeroes = true;
	}
	if (n > 10 || needZeroes) {
		lcd_write_char('0' + (n / 10));
		n %= 10;
	}
	lcd_write_char('0' + n);
}

byte lcd_flush_step(SoftwareSerial serial) {
	if (lcd_buffer.empty()) {
		return 0;
	}
	byte b = lcd_buffer.get();
	bool isInstruction = (b == 254);
	serial.write(b);
	if (!isInstruction || lcd_buffer.empty()) {
		return 1;
	}
	serial.write(lcd_buffer.get());
	return 2;
}

void lcd_flush_bytes(SoftwareSerial serial, byte count) {
	byte written = 0;
	while (written < count && !lcd_buffer.empty()) {
		written += lcd_flush_step(serial);
	}
}
