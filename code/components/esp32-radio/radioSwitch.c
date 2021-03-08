#include "stdio.h"
#include "startRadio.h"
#include "radioSwitch.h"

void radioSwitch(char channel)
{    
    switch (channel)
    {
        case '538':
            startRadio("https://20103.live.streamtheworld.com/TLPSTR09.mp3");
            break;

        case 'Q':
            startRadio("https://icecast-qmusicnl-cdp.triple-it.nl/Qmusic_nl_live_96.mp3");
            break;

        case 'SKY':
            startRadio("https://19993.live.streamtheworld.com/SKYRADIO.mp3");
            break;
    }
}