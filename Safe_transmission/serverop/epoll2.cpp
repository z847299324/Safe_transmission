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
#include"epoll2.h"

//线程结构体
/*
struct prcessdata
{
    int tfd;
    int epfd;
    epollrun run;
};
*/
int Epoll::senddata(int fd,char*data,int len,int epfd)
{
    int packlen = (len)+(sizeof(int));
    char* package = (char*)malloc(packlen);
    memset(package,0,packlen);
    char* tp = package;
    char* tl = (char*)&len;
    int i=0; 
    for(i=0;i<(sizeof(int));i++)
    {
        tp[i]=tl[i];
    }
    for(;i<packlen;i++)
    {
        tp[i] = *data++;
    }
    int ret = send(fd,package,packlen,0);
    if(ret == -1)
    {
        epoll_ctl(epfd,EPOLL_CTL_DEL,fd,0); 
        close(fd);
        return -1;
    }
    return 0;
}


int Epoll::recvdata(int fd,char**buf,int epfd)
{
    //先读一个包头
    char clen[4]={0};
    int ret = recv(fd,clen,4,0);
    if(ret == 0)
    {
        //客户端断开连接 下树 关闭客户端
        epoll_ctl(epfd,EPOLL_CTL_DEL,fd,0);
        close(fd);
        return 0;
    }
    int len = *(int*)clen;
    //如果是空包 立刻回复一个空包
    if(len == 0)
    {
        std::cout<<"12345"<<std::endl;
        senddata(fd,NULL,0,epfd); 
    }
    int i=0;
    char* tbuf =(char*)malloc(len);
    char* temp = tbuf;
    memset(tbuf,0,len);
    for(i=0;i<len;i++)
    {
        ret = recv(fd,temp,1,0);
        if(ret == 0)
        {
            //客户端断开连接 下树 关闭客户端
            epoll_ctl(epfd,EPOLL_CTL_DEL,fd,0);
            close(fd);
        }
        temp++;
    }
    *buf = tbuf;
    return len; 
}



//提供给用户的回调接口
void*func(Epoll*ep)
{
    char* buf = NULL;
    int len = ep->recvdata(ep->fd,&buf,ep->epfd);
    ep->senddata(ep->fd,buf,len,ep->epfd);   

    return NULL;

}



//线程业务
void* process(void*data)
{
    printf("process\n");
    Epoll * pdata = (Epoll*)data;
    pdata->run(pdata);
    return NULL;
}

void Epoll::initepoll(int port,char* ip,epollrun epdata)
{
    //创建监听套接字
    int lfd = socket(AF_INET,SOCK_STREAM,0);
    if (lfd == -1)
    {
        perror("error socket");
    }
    //设置端口复用
    int opt = 1;
		setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    
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
    this->epfd = epoll_create(1024);
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
                this->fd = events[i].data.fd;
                this->run = epdata;
                //组织结构体
                /*struct prcessdata s;
                s.tfd = tfd;
                s.epfd = epfd;
                s.run = epdata;
              	printf("s.run\n");
                */
                //线程池中添加任务
                threadpool_add(thp, process, (void*)this); 

            }
        }
    }

    close(lfd);
}
