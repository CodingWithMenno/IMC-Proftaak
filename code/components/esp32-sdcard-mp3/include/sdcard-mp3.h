#ifndef SDCARD_MP3_H
#define SDCARD_MP3_H

void mp3_task(void*);
void mp3_stopTask();
void mp3_addToQueue(char*);
void mp3_play(char*);
void mp3_stop();

#endif