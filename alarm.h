#define ALARM_HAS_ALARM 0x1
#define ALARM_SOUND_ENABLED 0x2
#define ALARM_CAN_TRIGGER 0x4
uint8_t alarm_state = ALARM_CAN_TRIGGER;

uint8_t alarm_temp = 255;

bool alarm_active() {
	return (alarm_state & ALARM_HAS_ALARM) > 0;
}

void alarm_set() {
	alarm_state |= ALARM_HAS_ALARM;
	alarm_state &= ~ALARM_CAN_TRIGGER;
}

void alarm_clear() {
	alarm_state &= ~ALARM_HAS_ALARM;
	led_clear_all();
	digitalWrite(PIN_ALARM, LOW);
}

void alarm_time_step() {
	if ((alarm_state & ALARM_CAN_TRIGGER) > 0 && tempCurrent >= alarm_temp) {
		alarm_set();
	} else if (tempCurrent < alarm_temp) {
		alarm_state |= ALARM_CAN_TRIGGER;
	}
}

bool alarm_is_sound_enabled() {
	return (alarm_state & ALARM_SOUND_ENABLED) > 0;
}

void alarm_set_sound_enabled(bool enabled) {
	if (enabled) {
		alarm_state |= ALARM_SOUND_ENABLED;
	} else {
		alarm_state &= ~ALARM_SOUND_ENABLED;
	}
}

void save_alarm_data() {
	mem_write_start_time(&dtStart);
	mem_write_latest_time(&dtCurrent);
	mem_write_time_power_off(&tsPowerOff);
	mem_write_min_max_temp(tempMin, tempMax);
	mem_write_time_over_temp(&tsOverTemp);
}

void alarm_reset() {
	dtStart = dtCurrent = rtc.now();
	tsPowerOff = tsOverTemp = TimeSpan(0);
	tempMin = tempMax = tempCurrent = read_temp_uint8();
	alarm_clear();
	save_alarm_data();
}

void alarm_show_current_temp() {
	LCD_COMMAND(bpi_line1);
	lcd_write_string(F("Alarm temp: "));
	lcd_write_integer(alarm_temp);
	lcd_write_4_spaces();
	if (!alarm_is_sound_enabled()) {
		lcd_write_char(254);
		lcd_write_char(128 + 15);
		lcd_write_char(235);
	}
	lcd_set_temporary_message();
}
