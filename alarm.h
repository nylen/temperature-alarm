#define ALARM_HAS_ALARM 0x1
#define ALARM_HAD_ALARM_LAST_TIME_STEP 0x2
uint8_t alarm_state = 0;

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
