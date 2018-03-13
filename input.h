#define INPUT_IDLE 0
#define INPUT_WAITING_DIGIT_1 1
#define INPUT_WAITING_DIGIT_2 2

#define MS_INPUT_EXPIRE_TIME 3000

byte input_state = 0;

uint32_t input_started = 0;
byte input_val = 0;

void input_write_input_val() {
	switch (input_state) {
		case INPUT_WAITING_DIGIT_1:
			lcd_write_integer(input_val / 10);
			break;
		case INPUT_WAITING_DIGIT_2:
			if (input_val < 10) {
				lcd_write_char('0');
			}
			lcd_write_integer(input_val);
			break;
	}
}

void input_write_first_message() {
	LCD_COMMAND(bpi_line2);
	lcd_write_string(F("Input: "));
	input_write_input_val();
	lcd_write_16_spaces();
}

void input_write_incremental_message() {
	// Position cursor after "Input: "
	lcd_write_char(254);
	lcd_write_char(192 + 7);
	input_write_input_val();
}

void input_mode_enable() {
	input_state = INPUT_WAITING_DIGIT_1;
	input_val = 0;
	input_started = millis();
	input_write_first_message();
}

bool input_mode_active() {
	return (input_state > 0);
}

void input_time_step() {
	if (input_state == INPUT_IDLE) {
		return;
	}

	if (is_button_pressed()) {
		input_started = millis();
	}

	switch (input_state) {
		case INPUT_WAITING_DIGIT_1:
			if (was_button_pressed_short()) {
				clear_button_press();
				input_val += 10;
				if (input_val == 100) {
					input_val = 0;
				}
				input_write_incremental_message();
			} else if (was_button_pressed_long()) {
				clear_button_press();
				input_state = INPUT_WAITING_DIGIT_2;
				input_write_incremental_message();
			} else if (millis() - input_started > MS_INPUT_EXPIRE_TIME) {
				// This input has expired
				input_state = INPUT_IDLE;
				LCD_COMMAND(bpi_line2);
				lcd_write_16_spaces();
			}
			break;

		case INPUT_WAITING_DIGIT_2:
			if (was_button_pressed_short()) {
				clear_button_press();
				if (input_val % 10 == 9) {
					input_val -= 9;
				} else {
					input_val++;
				}
				input_write_incremental_message();
			} else if (was_button_pressed_long()) {
				clear_button_press();
				// Reset the input state
				input_state = INPUT_IDLE;
				// Commit this input
				// A value of zero has a special meaning: toggle alarm sound
				if (input_val) {
					// Save the input value as the alarm temperature
					alarm_temp = input_val + TEMP_OFFSET;
					mem_write_alarm_temp(alarm_temp);
					// Display the new alarm temperature
					alarm_show_current_temp();
				} else {
					// Toggle alarm sound
					alarm_set_sound_enabled(!alarm_is_sound_enabled());
					mem_write_sound_enabled(alarm_is_sound_enabled());
					LCD_COMMAND(bpi_line1);
					if (alarm_is_sound_enabled()) {
						lcd_write_string(F("Sound enabled"));
					} else {
						lcd_write_string(F("Sound disabled"));
					}
					lcd_write_4_spaces();
					lcd_set_temporary_message();
				}
				LCD_COMMAND(bpi_line2);
				lcd_write_16_spaces();
			} else if (millis() - input_started > MS_INPUT_EXPIRE_TIME) {
				// This input has expired
				input_state = INPUT_IDLE;
				LCD_COMMAND(bpi_line2);
				lcd_write_16_spaces();
			}
			break;
	}
}
