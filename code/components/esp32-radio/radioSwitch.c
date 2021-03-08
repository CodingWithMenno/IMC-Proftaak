#include "stdio.h"
#include "string.h"
#include "startRadio.h"
#include "radioSwitch.h"

void radioSwitch(char channel[])
{    
    char* Ip;
    char Ipa;

    if (strcmp(channel, "538") == 0)
    {
        Ip = "https://20103.live.streamtheworld.com/TLPSTR09.mp3";
        Ipa = *Ip;
        startRadio(Ipa);
    }
    else if(strcmp(channel, "Q") == 0)
    {
        Ip = "https://icecast-qmusicnl-cdp.triple-it.nl/Qmusic_nl_live_96.mp3";
        Ipa = *Ip;
        startRadio(Ipa);
    }
    else if (strcmp(channel, "SKY") == 0)
    {
        Ip = "https://19993.live.streamtheworld.com/SKYRADIO.mp3";
        Ipa = *Ip;
        startRadio(Ipa);
    } 
}