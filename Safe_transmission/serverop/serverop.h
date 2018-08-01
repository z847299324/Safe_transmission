#pragma once
#include"epoll2.h"
#include"RequestCodec.h"
#include"RespondCodec.h"
class ServerInfo
{
public:
    char            serverID[12];   // 服务器端编号
    char            dbUse[24];      // 数据库用户名
    char            dbPasswd[24];   // 数据库密码
    char            dbSID[24];      // 数据库sid

    unsigned short  sPort;          // 服务器绑定的端口
    int             maxnode;        // 共享内存最大网点树 客户端默认1个
    int             shmkey;         // 共享内存keyid 创建共享内存时使用  
};

class serverop
{
public:
    serverop(ServerInfo *serInfo);
    ~serverop();
    //服务器开始工作
    void workstart();
    //秘钥协商
    int secMngArrange();
    friend void* working(int cfd,int epfd);
private:
    ServerInfo *sinfo;
    RequestMsg reqmsg;
};

