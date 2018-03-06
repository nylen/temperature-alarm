// Time steps (simple multitasking using time-slicing)

#define MS_TIME_STEP 50

#define STEPS_TEMP_READING 20

unsigned long time_step_start = 0;
unsigned long time_step_max_elapsed = 0;
uint32_t time_step_counter = 0;
#define TIME_STEP_WRAP (((uint32_t)20)*3600)

void time_step(bool record_max = true) {
	time_step_counter = (time_step_counter + 1) % TIME_STEP_WRAP;
	unsigned long ms = millis();
	unsigned long elapsed = ms - time_step_start;
	if (record_max && elapsed > time_step_max_elapsed) {
		time_step_max_elapsed = elapsed;
	}
	if (elapsed >= MS_TIME_STEP) {
		Serial.println("!");
	} else if (elapsed > 0) {
		delay(MS_TIME_STEP - (ms % MS_TIME_STEP));
	} else {
		delay(MS_TIME_STEP);
	}
	time_step_start = millis();
}
