#define ADDR_START_TIME      0 // 6 bytes
#define ADDR_LATEST_TIME     6 // 6 bytes
#define ADDR_TIME_POWER_OFF 12 // 4 bytes
#define ADDR_ALARM_TEMP     16 // 1 byte
#define ADDR_SOUND_ENABLED  17 // 1 byte
#define ADDR_MIN_MAX_TEMP   18 // 2 bytes

void _mem_read_DateTime(uint32_t addr, DateTime *dt) {
	uint16_t y = fram.read8(addr++);
	uint8_t m  = fram.read8(addr++);
	uint8_t d  = fram.read8(addr++);
	uint8_t hh = fram.read8(addr++);
	uint8_t mm = fram.read8(addr++);
	uint8_t ss = fram.read8(addr++);
	*dt = DateTime(y + 2000, m, d, hh, mm, ss);
}

void _mem_write_DateTime(uint32_t addr, DateTime *dt) {
	fram.writeEnable(true);
	fram.write8(addr++, dt->rawYear());
	// MB85RS64V datasheet says:
	// WEL (Write Enable Latch) is reset after the following operations: ...
	// At the rising edge of CS after WRITE command recognition
	fram.writeEnable(true);
	fram.write8(addr++, dt->month());
	fram.writeEnable(true);
	fram.write8(addr++, dt->day());
	fram.writeEnable(true);
	fram.write8(addr++, dt->hour());
	fram.writeEnable(true);
	fram.write8(addr++, dt->minute());
	fram.writeEnable(true);
	fram.write8(addr++, dt->second());
}

void mem_read_start_time(DateTime* dt) {
	_mem_read_DateTime(ADDR_START_TIME, dt);
}

void mem_write_start_time(DateTime *dt) {
	_mem_write_DateTime(ADDR_START_TIME, dt);
}

void mem_read_latest_time(DateTime *dt) {
	_mem_read_DateTime(ADDR_LATEST_TIME, dt);
}

void mem_write_latest_time(DateTime *dt) {
	_mem_write_DateTime(ADDR_LATEST_TIME, dt);
}

uint32_t _mem_read_uint32(uint32_t addr) {
	return (
		(uint32_t)fram.read8(addr + 0) << 24 |
		(uint32_t)fram.read8(addr + 1) << 16 |
		(uint32_t)fram.read8(addr + 2) <<  8 |
		(uint32_t)fram.read8(addr + 3)
	);
}

void _mem_write_uint32(uint32_t addr, uint32_t val) {
	fram.writeEnable(true);
	fram.write8(addr++, val >> 24 & 0xff);
	fram.writeEnable(true);
	fram.write8(addr++, val >> 16 & 0xff);
	fram.writeEnable(true);
	fram.write8(addr++, val >>  8 & 0xff);
	fram.writeEnable(true);
	fram.write8(addr++, val       & 0xff);
}

void mem_read_time_power_off(TimeSpan *ts) {
	*ts = TimeSpan((int32_t)_mem_read_uint32(ADDR_TIME_POWER_OFF));
}

void mem_write_time_power_off(TimeSpan *ts) {
	_mem_write_uint32(ADDR_TIME_POWER_OFF, (uint32_t)ts->totalseconds());
}

byte mem_read_alarm_temp() {
	return fram.read8(ADDR_ALARM_TEMP);
}

void mem_write_alarm_temp(uint8_t temp) {
	fram.writeEnable(true);
	fram.write8(ADDR_ALARM_TEMP, temp);
}

bool mem_read_sound_enabled() {
	return (fram.read8(ADDR_SOUND_ENABLED) != 0);
}

void mem_write_sound_enabled(bool enabled) {
	fram.writeEnable(true);
	fram.write8(ADDR_SOUND_ENABLED, enabled ? 1 : 0);
}

void mem_read_min_max_temp(uint8_t *tempMin, uint8_t *tempMax) {
	*tempMin = fram.read8(ADDR_MIN_MAX_TEMP);
	*tempMax = fram.read8(ADDR_MIN_MAX_TEMP + 1);
}

void mem_write_min_max_temp(uint8_t tempMin, uint8_t tempMax) {
	fram.writeEnable(true);
	fram.write8(ADDR_MIN_MAX_TEMP, tempMin);
	fram.writeEnable(true);
	fram.write8(ADDR_MIN_MAX_TEMP + 1, tempMax);
}
