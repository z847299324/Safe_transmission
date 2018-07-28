#pragma once
#include"shmClass.h"
#include<iostream>
#include<string.h>
#include<stdlib.h>
using namespace std;
class keyinfo
{
public:
    int             status;         //密钥状态 0-有效 1无效
    char            clientId[12];   //客户端id
    char            serverId[12];   //服务器端id    
    int             seckeyid;       //对称密钥id
    char   seckey[128];    //对称密钥 //hash1 hash256 md5
};

class shmkey
{
public:
    //创建共享内存
    shmkey(int key,int maxnode);
    //存储数据
    int shmPutdata(keyinfo*keydata);
    //取出数据
    keyinfo* shmGetdata(char* cliendId,char* serverId,int seckeyId);
    ~shmkey() {}

private:
    int maxnode;
    shmClass *shm;
};

