#pragma once
#include "RequestCodec.h"
#include "cli.h"
#include "RespondCodec.h"
class ClientInfo
{
public:
    char   clientId[12];   //客户端编号
    char   AuthCode[34];   //认证码
    char   serverId[12];   //服务器端编号

    char   serverip[32];
    int    serverport;

    int    maxnode;        //最大网点数 客户端默认1个
    int    shmkey;         //共享内存keyid 创建共享内存时使用   
    int    shmhdl;         //共享内存句柄  
};



class clientop
{
public:
    //初始化客户端信息类
    clientop();
    ~clientop();
    //秘钥协商
     int keyMngArrange();
    //秘钥校验
    

     char* getKey();
private:
    ClientInfo cinfo;
    char Key[34];
};

