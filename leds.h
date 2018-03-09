byte led_step_counter = 0;
bool led_step_up = true;

void led_set_all(uint8_t r, uint8_t g, uint8_t b) {
	for (byte i = 0; i < NUM_LEDS; i++) {
		strip.setPixelColor(i, r, g, b);
	}
}

void led_step() {
	led_set_all(0, 0, 0);
	strip.setPixelColor(led_step_counter, 255, 100, 0);
	strip.show();
	if (led_step_up) {
		if (++led_step_counter == NUM_LEDS - 1) {
			led_step_up = false;
		}
	} else {
		if (--led_step_counter == 0) {
			led_step_up = true;
		}
	}
}
