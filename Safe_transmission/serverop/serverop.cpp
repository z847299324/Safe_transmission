#include <iostream>
#include "serverop.h"
#include <string.h>
#include <stdlib.h>
using namespace std;
serverop::serverop(ServerInfo *serInfo)
{   
    this->sinfo = new ServerInfo;
    //初始化server结构体信息
    memcpy(sinfo,serInfo,sizeof(ServerInfo));
}
serverop::~serverop()
{

}
void *working(Epoll* ep)
{

    char* buf = NULL;
    serverop *sop = new serverop(ep->sinfo);
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
    RequestMsg* reqmsg = (RequestMsg*)reqcod.msgDecode(buf,len);
    switch(reqmsg->cmdType)
    {
        case 1:
            //秘钥协商
            sop->secMngArrange();
            break;
        case 2:
            break;
        case 3:
            break;
    }

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
int serverop::secMngArrange()
{
    cout<<666<<endl;
    return 0;
}
