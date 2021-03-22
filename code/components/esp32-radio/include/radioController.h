#ifndef RADIOCONTROLLER_H
#define RADIOCONTROLLER_H

void radio_init();
void radio_stop();
void radio_switch(char[]);
void radio_task(void*);
void radio_reset();
void radio_quit();

#endif
