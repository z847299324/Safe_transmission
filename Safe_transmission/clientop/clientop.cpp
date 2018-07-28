#include <iostream>
#include "clientop.h"
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

clientop::clientop()
{
    strcpy(this->cinfo.clientId,"1111");
    strcpy(this->cinfo.serverId,"1111");
    strcpy(this->cinfo.serverip,"127.0.0.1");
    this->cinfo.serverport = 8989;
    this->cinfo.maxnode = 1;
    this->cinfo.shmkey = 11;
}

//秘钥协商
int clientop::keyMngArrange()
{
    //组织秘钥请求结构体
    RequestMsg reqmsg;
    reqmsg.cmdType = 1;
    strcpy(reqmsg.clientId,this->cinfo.clientId);
    //秘钥随机数
    int i =0;
    for(i=0;i<30;i++)
    {
        reqmsg.r1[i] = rand()%10+'0';    
    }
    //md5认证码
    char data[64]={0};
    char tmp[32] = {0};
    strcat(data,reqmsg.clientId);
    strcat(data,reqmsg.serverId);
    strcat(data,reqmsg.r1);
    MD5((unsigned char*)data,strlen(data),(unsigned char*)tmp);
    for(i=0;i<16;i++)
    {
        sprintf(&reqmsg.authCode[i*2],"%02x",tmp[i]);
    }
    //报文编码
    char* codData=NULL;
    int eDatalen;
    RequestCodec reqEncod(&reqmsg);
    reqEncod.msgEncode(&codData,eDatalen);
    //socket连接
    int fd=initcli(cinfo.serverip,cinfo.serverport);
    senddata(fd,codData,eDatalen);
    eDatalen = recvdata(fd,&codData);
    //报文解码
    RespondCodec resDecod;
    RespondMsg* resmsg =(RespondMsg*)resDecod.msgDecode(codData,eDatalen);
    if(strcmp(resmsg->clientId,cinfo.clientId)==0&&strcmp(resmsg->serverId,cinfo.serverId)==0&&resmsg->rv==0)
    {
        //拿到秘钥随机数 组成秘钥
        int klen = strlen(reqmsg.r1)+strlen(resmsg->r2);  
        for(i=0;i<klen;i++)
        {

        }
    }

    return 0;
}

