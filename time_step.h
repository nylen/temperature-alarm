// Time steps (simple multitasking using time-slicing)

#define MS_TIME_STEP 50

#define STEPS_TEMP_READING 20
#define STEPS_SAVE_TIME_INFO (STEPS_TEMP_READING * 5)

#define STEPS_SWITCH_UP_DOWN (STEPS_TEMP_READING * 2)
#define STEPS_SWITCH_SOUND_ON_OFF (STEPS_TEMP_READING / 2)

uint32_t time_step_start = 0;
uint32_t time_step_counter = 0;
#define TIME_STEP_WRAP (((uint32_t)20)*3600)

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
