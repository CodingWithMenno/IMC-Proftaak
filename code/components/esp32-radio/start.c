#include "ipChange.h"
#include "startRadio.h"

void app_main(){
    setIp("https://icecast.omroep.nl/radio1-bb-mp3");
    startRadio();
}