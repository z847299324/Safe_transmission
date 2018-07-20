#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>          /* See NOTES */
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<unistd.h>
#include <fcntl.h>
#include"threadpool.h"
#define EVENTS 1024
typedef void*(*epollrun)(int,int);
//线程结构体
struct prcessdata
{
    int tfd;
    int epfd;
    epollrun run;
};


//提供给用户的回调接口
void*func(int tfd,int epfd)
{
    printf("xxx\n");
    char buf[2] = {0};
    int clen;
    while((clen=recv(tfd,buf,sizeof(buf),0))!=-1)
    {
        printf("xxx\n");
        if(clen == 0)
        {
            //客户端断开连接 下树 关闭客户端
            epoll_ctl(epfd,EPOLL_CTL_DEL,tfd,0);
            close(tfd);
        }
        else
        {
            printf("sss\n");
            //正常
            send(tfd,buf,clen,0);

        }
    }
    return NULL;
}
/*
//读取数据函数
int recvdata(int tfd,char*buf,blen)
{

}

//发送数据函数
int senddata(int tfd,char*buf,datalen)
{

}
*/
//线程业务
void* process(void*data)
{
            printf("process\n");
    struct prcessdata * pdata = (struct prcessdata*)data;
    pdata->run(pdata->tfd,pdata->epfd);
    return NULL;
}

void initepoll(int port,char* ip,void*(epdata)(int,int))
{
    //创建监听套接字
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if (lfd == -1)
    {
        perror("error socket");
    }
    //初始化结构体
    struct sockaddr_in seve;
    seve.sin_family = AF_INET;
    seve.sin_port = htons(port);
    inet_pton(AF_INET,ip,&seve.sin_addr.s_addr);
    //绑定
    int ret = bind(lfd,(struct sockaddr*)&seve,sizeof(seve));
    if(ret == -1)
    {
        perror("error bind");
    }
    //启动侦听
    ret = listen(lfd,1024);
    if(ret == -1)
    {
        perror("error listen");
    }
    //初始化epoll树
    int epfd = epoll_create(1024);
    if(epfd == -1)
    {
        perror("error epoll_create");
    }
    //初始化epoll结构体
    struct epoll_event ev;
    struct epoll_event events[EVENTS];
    memset(events,0,EVENTS);
    ev.events = EPOLLIN;
    ev.data.fd = lfd;
    //监听描述符上树
    epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&ev);
    //初始化客户端信息结构体
    struct sockaddr_in cil;
    socklen_t cillen = sizeof(cil);
    //初始化线程池
    threadpool_t *thp = threadpool_create(3,10,10);
    //循环等待
    while(1)
    {
        printf("dsadadas\n");
        int nfd = epoll_wait(epfd,events,EVENTS,-1);
        printf("nfd:%d\n",nfd);
        if(nfd == -1)
        {
            perror("error epoll_wait");
        }
        int i = 0;
        for(i=0;i<nfd;i++)
        {
            //有新连接请求
            if(events[i].data.fd == lfd)
            {
                printf("有新连接\n");
                int cfd = accept(lfd,(struct sockaddr*)&cil,&cillen);
                if(cfd == -1)
                {
                    perror("error accept");
                }
                //设置cfd非堵塞
                int flag = fcntl(cfd,F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd,F_SETFL,flag);
                //               printf("%d,%s\n",ntohs(cil.sin_port),inet_ntoa(cil.sin_addr));
                //上树
                ev.events = EPOLLIN|EPOLLET;
                ev.data.fd = cfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&ev);
            }
            else
            {
                int tfd = events[i].data.fd;
                //组织结构体
                struct prcessdata s;
                s.tfd = tfd;
                s.epfd = epfd;
                s.run = epdata;
                printf("s.run\n");
                //线程池中添加任务
                threadpool_add(thp, process, (void*)&s); 

                /*
                //需要通信 
                char buf[1024] = {0};
                int clen = recv(tfd,buf,sizeof(buf),0);
                if(clen == 0)
                {
                //客户端断开连接 下树 关闭客户端
                epoll_ctl(epfd,EPOLL_CTL_DEL,tfd,0);
                close(tfd);
                }
                else if(clen == -1)
                {
                //发生错误 下树 关闭客户端
                epoll_ctl(epfd,EPOLL_CTL_DEL,tfd,0);
                close(tfd);
                }
                else
                {
                //正常
                send(tfd,buf,clen,0);
                }
                */
            }
        }
    }

    close(lfd);
}


int main()
{
    initepoll(8080,"127.0.0.1",func);
    return 0;
}
