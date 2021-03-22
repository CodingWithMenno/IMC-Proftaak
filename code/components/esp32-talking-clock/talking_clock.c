
#include "talking_clock.h"
#include "sdcard-mp3.h"
#include <string.h>
#include <time.h>
#include <math.h>
	
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "sdcard-mp3.h"

esp_err_t talkingClock_fillQueue() 
{
	time_t now;
    struct tm timeinfo;
    time(&now);
	
	char strftime_buf[64];
	localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

	// Convert hours to AM/PM unit
	int hour = timeinfo.tm_hour;
	hour = (hour == 0 ? 12 : hour);
	hour = (hour > 12 ? hour%12 : hour);

	int minute = timeinfo.tm_min;

	// Het is
	mp3_addToQueue(talkingClock_files[TALKING_CLOCK_ITSNOW_INDEX]);

	// Het uur
	int hourId = hour == 0 ? 14 : (hour + 2);
	mp3_addToQueue(talkingClock_files[hourId]);
	mp3_addToQueue(talkingClock_files[TALKING_CLOCK_HOUR_INDEX]);
	if (minute == 0)
	{
		return ESP_OK;
	} else if (minute % 10 == 0)
	{
		int minuteId = minute / 10 + 15;
		mp3_addToQueue(talkingClock_files[minuteId]);
	} else if (minute < 20)
	{
		int unitId = (minute % 10) + 2;
		mp3_addToQueue(talkingClock_files[unitId]);

		if (minute > 10)
			mp3_addToQueue(talkingClock_files[TALKING_CLOCK_10_INDEX]);
	} else
	{
		int unitId = (minute % 10) + 2;
		mp3_addToQueue(talkingClock_files[unitId]);

		mp3_addToQueue(talkingClock_files[TALKING_CLOCK_AND_INDEX]);

		int tenId = floor(minute / 10.0) + 15;
		mp3_addToQueue(talkingClock_files[tenId]);
	}
	
	return ESP_OK;
}


