uint32_t button_pressed_start = 0;

#define MS_BUTTON_PRESS_LONG 1000

#define BUTTON_IGNORE_UNTIL_UP 255
#define BUTTON_IDLE 0
#define BUTTON_DOWN 1
#define BUTTON_DOWN_LONG 2
#define BUTTON_PRESSED_SHORT 3
#define BUTTON_PRESSED_LONG 4
uint8_t button_state = 0;

bool is_button_pressed() {
	byte presses = 0;
	if (digitalRead(PIN_BUTTON) == LOW) presses++; // 1
	if (digitalRead(PIN_BUTTON) == LOW) presses++; // 2
	if (digitalRead(PIN_BUTTON) == LOW) presses++; // 3
	if (digitalRead(PIN_BUTTON) == LOW) presses++; // 4
	if (digitalRead(PIN_BUTTON) == LOW) presses++; // 5
	return presses >= 3;
}

void button_time_step() {
	switch (button_state) {
		case BUTTON_IGNORE_UNTIL_UP:
			if (!is_button_pressed()) {
				button_state = BUTTON_IDLE;
			}
			break;
		case BUTTON_IDLE:
			if (is_button_pressed()) {
				button_state = BUTTON_DOWN;
				button_pressed_start = millis();
			}
			break;
		case BUTTON_DOWN:
			if (is_button_pressed()) {
				if (millis() - button_pressed_start > MS_BUTTON_PRESS_LONG) {
					button_state = BUTTON_DOWN_LONG;
				}
			} else {
				button_state = BUTTON_PRESSED_SHORT;
				button_pressed_start = 0;
			}
			break;
		case BUTTON_DOWN_LONG:
			if (!is_button_pressed()) {
				button_state = BUTTON_PRESSED_LONG;
				button_pressed_start = 0;
			}
			break;
	}
}

bool was_button_pressed_short() {
	return (button_state == BUTTON_PRESSED_SHORT);
}

bool was_button_pressed_long() {
	return (button_state == BUTTON_PRESSED_LONG || button_state == BUTTON_DOWN_LONG);
}

void clear_button_press() {
	button_state = BUTTON_IGNORE_UNTIL_UP;
	button_pressed_start = 0;
}
