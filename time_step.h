// Actions taken every N seconds (ensure that N % 60 == 0)
#define SECONDS_SAVE_TIME_INFO 5

// Time-based messages
#define SECONDS_SWITCH_TIME_MESSAGE 2
#define TIME_MESSAGE_ON  0
#define TIME_MESSAGE_OFF 1
#define TIME_MESSAGE_HOT 2

uint8_t second_step_counter = 0;

uint8_t second_step_message_index() {
	second_step_counter = (second_step_counter + 1) % (
		SECONDS_SWITCH_TIME_MESSAGE * (
			1 +
			(tsPowerOff.totalseconds() > 0 ? 1 : 0) +
			(tsOverTemp.totalseconds() > 0 ? 1 : 0)
		)
	);
	if (second_step_counter < SECONDS_SWITCH_TIME_MESSAGE) {
		return TIME_MESSAGE_ON;
	} else if (second_step_counter < 2 * SECONDS_SWITCH_TIME_MESSAGE) {
		// We are showing 2 or 3 messages.
		if (tsPowerOff.totalseconds() > 0) {
			return TIME_MESSAGE_OFF;
		} else {
			return TIME_MESSAGE_HOT;
		}
	} else {
		// We are showing all 3 messages.
		return TIME_MESSAGE_HOT;
	}
}

// Time steps (simple multitasking using time-slicing)

#define MS_TIME_STEP 50

#define STEPS_SWITCH_SOUND_ON_OFF 10

uint32_t time_step_start = 0;
uint8_t time_step_counter = 0;
#define TIME_STEP_WRAP 100

void time_step() {
	time_step_counter = (time_step_counter + 1) % TIME_STEP_WRAP;
	uint32_t ms = millis();
	uint32_t elapsed = ms - time_step_start;
	if (elapsed >= MS_TIME_STEP) {
		Serial.println("!");
	} else if (elapsed > 0) {
		delay(MS_TIME_STEP - (ms % MS_TIME_STEP));
	} else {
		delay(MS_TIME_STEP);
	}
	time_step_start = millis();
}
