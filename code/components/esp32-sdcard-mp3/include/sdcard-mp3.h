#ifndef SDCARD_MP3_H
#define SDCARD_MP3_H

static const char *mp3_file[] = {
    "/sdcard/test.mp3",
    "/sdcard/test1.mp3",
    "/sdcard/test2.mp3",
};
// more files may be added and `MP3_FILE_COUNT` will reflect the actual count
#define MP3_FILE_COUNT sizeof(mp3_file)/sizeof(char*)


#define CURRENT 0
#define NEXT    1

void mp3_init();
void mp3_update();
void mp3_start();
void mp3_stop();

#endif