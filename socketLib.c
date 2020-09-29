#include <stdlib.h>
#include <iostream>

using namespace std;

#include "socketLib.h"

struct CMMP
{
    int type;
    char message;
};


void setCMMP(int t, char msg)
{
    type = t;
    message = msg;
}

CMMP getCMMP()
{
    return CMMP;
}