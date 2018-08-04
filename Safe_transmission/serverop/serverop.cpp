#include <iostream>
#include "serverop.h"
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>
using namespace std;
serverop::serverop(ServerInfo *serInfo)
{   
    this->sinfo = new ServerInfo;
    //初始化server结构体信息
    memcpy(sinfo,serInfo,sizeof(ServerInfo));
    strcpy(this->Key,sinfo->serverID);
}
serverop::~serverop()
{

}
void *working(Epoll* ep)
{

    char* buf = NULL;
    serverop sop(ep->sinfo);
    //读取数据
    int len;
    len = ep->recvdata(ep->fd,&buf,ep->epfd);
    if(len == 0)
    {
        cout<<"Connection interruption"<<endl;
        return NULL;
    }
    //解码
    RequestCodec reqcod;
    sop.reqmsg = (RequestMsg*)reqcod.msgDecode(buf,len);
    //需要发送的信息
    char* data=NULL;
    int reslen;
    switch(sop.reqmsg->cmdType)
    {
    case 1:
        //秘钥协商
        sop.secMngArrange(&data,&reslen);
        break;
    case 2:
        break;
    case 3:
        break;
    }
   //回发
    ep->senddata(ep->fd,data,reslen,ep->epfd);   
    return NULL;
}
//服务器开始工作
void serverop::workstart()
{
    Epoll *ep=new Epoll;
    ep->sinfo = new ServerInfo;
    ep->sinfo=sinfo;
    //memcpy(ep->sinfo,this->sinfo,sizeof(ServerInfo));
    ep->initepoll(sinfo->sPort,"127.0.0.1",working);
}
//秘钥协商
int serverop::secMngArrange(char**coddata,int *codlen)
{
    //秘钥协商 得到结构体 校验信息 组织认证码
    char data[128]={0};
    char tmp[34] = {0};
    char authcd[34] = {0};
    strcat(data,reqmsg->clientId);
    strcat(data,reqmsg->serverId);
    strcat(data,reqmsg->r1);
    strcat(data,Key);
    MD5((unsigned char*)data,strlen(data),(unsigned char*)tmp);
    int i=0;
    for(i=0;i<16;i++)
    {
        sprintf(&authcd[i*2],"%02x",tmp[i]);
    }
    //初始化秘钥应答结构体
    this->resmsg = new RespondMsg;
    strcpy(resmsg->serverId ,this->sinfo->serverID);
    do{
        //对比认证码
        if(strcmp(this->reqmsg->authCode,authcd)!=0)
        {
            resmsg->rv=-1;
            break;
        }
        if(strcmp(this->sinfo->serverID,this->reqmsg->serverId)!=0)
        {
            resmsg->rv = -2;
            break;
        }
        resmsg->rv = 0;
        strcpy(resmsg->clientId,this->reqmsg->clientId);
        //秘钥编号
        //生成一半秘钥
        for(i=0;i<30;i++)                                         
        {                                                         
            resmsg->r2[i] = rand()%26+'a';                         
        }                              
        //合成秘钥
        char r1r2[64]={0};
        int klen = strlen(this->reqmsg->r1)+strlen(resmsg->r2);  
        for(i=0;i<klen;i++)
        {   
            r1r2[i*2]=reqmsg->r1[i];
            r1r2[i*2+1]=resmsg->r2[i];
        }   
        memset(tmp,0,32);
        MD5((unsigned char*)r1r2,strlen(r1r2),(unsigned char*)tmp);
        memset(Key,0,34);
        for(i=0;i<16;i++)
        {   
            sprintf(&this->Key[i*2],"%02x",tmp[i]);
        }
    }while(0);
    //编码
    RespondCodec rescod(resmsg);
    rescod.msgEncode(coddata,*codlen);
 
    return 0;
}
