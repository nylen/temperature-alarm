// from http://atlas-scientific.com/_files/code/ENV-TMP-Arduino-Sample-Code.pdf
// for further improvements: http://www.avrfreaks.net/forum/one-digit
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

	// convert Celsius to Fahrenheit
	temp = temp * 1.8 + 32;

	return temp;
}

// NOTE: Shifted by TEMP_OFFSET degrees!
// If this function returns 0 then the temperature is -TEMP_OFFSET!
uint8_t read_temp_uint8() {
	double temp = read_temp() + TEMP_OFFSET;
	if (temp <= 0) {
		return 0;
	}
	if (temp >= 255) { // something went badly wrong!
		return 255;
	}
	return lround(temp);
}
