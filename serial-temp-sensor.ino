#define PIN_TMP_POWER 2
#define PIN_TMP_DATA  A7

// from http://atlas-scientific.com/_files/code/ENV-TMP-Arduino-Sample-Code.pdf
double read_temp() {
	double v_out;
	double temp;

	// set pull-up on analog pin
	digitalWrite(PIN_TMP_DATA, LOW);

	// turn on temp sensor
	digitalWrite(PIN_TMP_POWER, HIGH);
	// wait for temp to stabilize (at least 1 ms)
	delay(2);
	// read the input pin
	v_out = analogRead(PIN_TMP_DATA);
	// turn off temp sensor
	digitalWrite(PIN_TMP_POWER, LOW);

	// convert ADC points to volts (.0048 ~= 5v / 1024pts)
	v_out *= .0048;
	// convert volts to millivolts
	v_out *= 1000;
	// convert millivolts to temperature
	temp = 0.0512 * v_out - 20.5128;

	return temp;
}

void setup() {
	// Set up for data transfer to computer
	Serial.begin(57600);
	// Notify listeners that we are ready
	Serial.println("ready");

	// Pin modes for temperature sensor
	pinMode(PIN_TMP_POWER, OUTPUT);
}

#define STATE_IDLE 0
#define STATE_RCV_T 1
#define STATE_RCV_E 2
#define STATE_RCV_M 3
#define STATE_RCV_P 4
#define STATE_RCV_NEWLINE 5

uint8_t state = STATE_IDLE;

void loop() {
	while (Serial.available()) {
		int c = Serial.read();
		if (state == STATE_IDLE && c == 'T') {
			state = STATE_RCV_T;
		} else if (state == STATE_RCV_T && c == 'E') {
			state = STATE_RCV_E;
		} else if (state == STATE_RCV_E && c == 'M') {
			state = STATE_RCV_M;
		} else if (state == STATE_RCV_M && c == 'P') {
			state = STATE_RCV_P;
		} else if (state == STATE_RCV_P && c == '\n') {
			state = STATE_RCV_NEWLINE;
		} else {
			state = STATE_IDLE;
		}
	}

	if (state == STATE_RCV_NEWLINE) {
		Serial.println(read_temp(), 5);
		state = STATE_IDLE;
	}

	delay(100);
}
