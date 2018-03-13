#define ALARM_HAS_ALARM 0x1
#define ALARM_HAD_ALARM_LAST_TIME_STEP 0x2
#define ALARM_SOUND_ENABLED 0x4
uint8_t alarm_state = 0;

uint8_t alarm_temp = 255;

bool alarm_active() {
	return (alarm_state & ALARM_HAS_ALARM) > 0;
}

bool alarm_was_active_last_time_step() {
	return (alarm_state & ALARM_HAD_ALARM_LAST_TIME_STEP) > 0;
}

void alarm_set() {
	alarm_state |= ALARM_HAS_ALARM;
}

void alarm_clear() {
	alarm_state &= ~ALARM_HAS_ALARM;
}

void alarm_time_step() {
	if (alarm_active()) {
		alarm_state |= ALARM_HAD_ALARM_LAST_TIME_STEP;
	} else {
		alarm_state &= ~ALARM_HAS_ALARM;
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
}

void alarm_reset() {
	dtStart = dtCurrent = rtc.now();
	tsPowerOff = TimeSpan(0);
	tempMin = tempMax = tempCurrent = read_temp_uint8();
	alarm_clear();
	save_alarm_data();
}
