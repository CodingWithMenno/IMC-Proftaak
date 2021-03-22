
#include "talking_clock.h"
#include "sdcard-mp3.h"
#include <string.h>
#include <time.h>
	
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "sdcard-mp3.h"

esp_err_t talkingClock_fillQueue() {
	
	time_t now;
    struct tm timeinfo;
    time(&now);
	
	char strftime_buf[64];
	localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

	int data = TALKING_CLOCK_ITSNOW_INDEX;
	mp3_addToQueue(talkingClock_files[data]);

	// Convert hours to AM/PM unit
	int hour = timeinfo.tm_hour;
	// int hour = 17;
	hour = (hour == 0 ? 12 : hour);
	hour = (hour > 12 ? hour%12 : hour);
	data = TALKING_CLOCK_1_INDEX-1+hour;

	//Add "Het is nu"
	mp3_addToQueue(talkingClock_files[data]);

	//Add 
	data = TALKING_CLOCK_HOUR_INDEX;
	mp3_addToQueue(talkingClock_files[data]);

	//SPLITTING MINUTES: F.E. 36 TO 30 AND 6
	int minute = timeinfo.tm_min;
	// int minute = 37;
	int first = minute;
	int second = timeinfo.tm_sec;
	// int second = minute % 10;
	while(first >= 10)
	{
		first = first / 10;
	}
	first *= 10;

	// printf("Minute:%i\t First:%i \t Second:%i\n",minute,first,second);

	if (minute > 14) { 					
		if (second == 0 ) {		// 20,30,40,50
			data = minute/10 + 15;
			mp3_addToQueue(talkingClock_files[data]);
		} else if(minute < 20) {		//15-19
			//Eerste digit
			data = second + 2;
			mp3_addToQueue(talkingClock_files[data]); 
			//Tien
			data = 12;
			mp3_addToQueue(talkingClock_files[data]);
		} else {						//De rest	
			//Eerste digit
			data = second + 2;
			mp3_addToQueue(talkingClock_files[data]);
			//AND
			data = TALKING_CLOCK_AND_INDEX;
			// printf("AND");
			mp3_addToQueue(talkingClock_files[data]);
			//Tiental
			data = minute/10 + 15;
			// printf("Minute:%i\n",data);
			mp3_addToQueue(talkingClock_files[data]);
		}
	} else if (minute != 0) {							//1-14
		data = minute + 2;
		// printf("Minute:%i\n",data);
		mp3_addToQueue(talkingClock_files[data]);
	}

	
	return ESP_OK;
}


