#include <iostream>
#include "serverop.h"
int main()
{
    ServerInfo *sInfo = new ServerInfo;
    strcpy(sInfo->serverID,"1111");
    sInfo->sPort = 8989;
    sInfo->maxnode = 1000;
    sInfo->shmkey = 001;
    serverop ser(sInfo);
    ser.workstart();   
    

    return 0;
}

