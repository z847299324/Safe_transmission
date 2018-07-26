#include <iostream>
#include "shmClass.h"
using namespace std;
#include"shmkey.h"
#include<string.h>
#include<stdlib.h>
using namespace std;
int main()
{
    keyinfo *info=new keyinfo;
    strcpy(info->clientId,"23451");
    info->status = 100;
    strcpy(info->serverId,"23451");
    info->seckeyid = 10010;
    strcpy(info->seckey,"23451");

    cout << info->clientId<<info->status << endl;
    
    //共享内存操作测试
    shmkey shmk(100,100);
    //存
    shmk.shmPutdata(info);

    keyinfo * datta = NULL;
    //取
    datta = shmk.shmGetdata(info->clientId,info->serverId,info->seckeyid);
    
    cout<<datta->clientId<<endl;
    return 0;
}

