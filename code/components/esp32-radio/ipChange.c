#include "string.h"
#include "ipChange.h"

static char *Ip;

void setIp(char* text)
{
    Ip = text;
}

char* getIp(void)
{
    return Ip;
}