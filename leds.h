// Bit 0x80: Set if stepping up; not set if stepping down
// Bits 0x7f: Current LED index
byte led_state = 0x80;

void led_set_all(uint8_t r, uint8_t g, uint8_t b) {
	for (byte i = 0; i < NUM_LEDS; i++) {
		strip.setPixelColor(i, r, g, b);
	}
}

void led_step() {
	led_set_all(0, 0, 0);
	strip.setPixelColor(led_state & 0x7f, 255, 100, 0);
	strip.show();
	if (led_state & 0x80) {
		if (++led_state == (0x80 | (NUM_LEDS - 1))) {
			led_state &= 0x7f;
		}
	} else {
		if (--led_state == 0) {
			led_state = 0x80;
		}
	}
}

void led_clear_all() {
	led_state = 0x80;
	led_set_all(0, 0, 0);
	strip.show();
}
