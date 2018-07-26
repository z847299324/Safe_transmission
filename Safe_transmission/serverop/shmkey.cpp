#include <iostream>
#include"shmkey.h"

//有参构造
shmkey::shmkey(int key,int maxnode)
{
    //创建共享内存
    shm =new shmClass(key,sizeof(keyinfo)*maxnode);   
    this->maxnode = maxnode;
}

//存储数据
int shmkey::shmPutdata(keyinfo * keydata)
{
    //关联共享内存
    keyinfo* head = static_cast<keyinfo*>(shm->getshm());
    keyinfo * info = head;
    //存数据  存之前判断是否存在  有则覆盖  无则往后追加
    int i = 0;
    for(i=0;i<maxnode;i++)
    {
        info=head+i;
        if(strcmp(keydata->clientId,((keyinfo*)info)->clientId)==0&&
           strcmp(keydata->serverId,((keyinfo*)info)->serverId)==0)
        {
            memset(info,0,sizeof(keyinfo));
            memcpy(info,keydata,sizeof(keyinfo));
            shm->deshm();
            return 0;
        }
    }
    keyinfo tmpinfo; //空节点
    memset(&tmpinfo,0,sizeof(keyinfo));
    for(i=0;i<maxnode;i++)
    {
        info=head+i;
        if(memcmp(&tmpinfo,info,sizeof(keyinfo))==0)
        {
            memset(info,0,sizeof(keyinfo));
            memcpy(info,keydata,sizeof(keyinfo));
            shm->deshm();
            return 0;
        }
    }
    if(i==maxnode)
        cout<<"shm full"<<endl;
    //断开共享内存
    shm->deshm();

    return 0;
}

//取出数.据
keyinfo* shmkey::shmGetdata(char* clientId,char*serverId,int seckeyId)
{
    //关联共享内存
    keyinfo* head =static_cast<keyinfo*>(shm->getshm());
    keyinfo * info = head;
    keyinfo * tdata = new keyinfo;
    memset(tdata,0,sizeof(keyinfo));
    int i=0;
    for(i=0;i<maxnode;i++)
    {
        info=head+i;
        if(strcmp(info->clientId,clientId)==0&&
           strcmp(info->serverId,serverId)==0&&
           info->seckeyid == seckeyId)
        {
            memcpy(tdata,info,sizeof(keyinfo));
            break;
        }
    }
    if(i==maxnode)
    {
        cout<<"not find"<<endl;
        return NULL;
    }
    shm->deshm();
    return tdata;
}
