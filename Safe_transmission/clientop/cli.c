#include <stdio.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <pthread.h>
struct pdata
{
    int cfd;
    struct sockaddr_in seve;
};

static pthread_mutex_t mtx_t;

int senddata(int fd,char* data,int len);

void* pthread(void*s)
{
    struct pdata * data = (struct pdata *)s;
    int flag = 0;
    char buf[10]={0};
    int ret = 0;
    while(1)
    {
        sleep(5);
        //发空包
        //加锁
        pthread_mutex_lock(&mtx_t);
        senddata(data->cfd,NULL,0);
        flag++;
        ret = recv(data->cfd,&buf,10,0);
        //解锁
        pthread_mutex_unlock(&mtx_t);
        if(ret>0)
        {
            flag = 0;
        }
        if(flag>=3)
        {
            //服务器断开连接 重新连接
            printf("Network connection interruption\n");
            //connect(data->cfd,(struct sockaddr*)&data->seve,sizeof(data->seve));
        }
    }
    return NULL;
}
int initcli(char*sip,int sport)
{
    int cfd = socket(AF_INET,SOCK_STREAM,0);
    if(cfd == -1)
    {
        perror("socket error");
    }

    struct sockaddr_in seve;
    seve.sin_family = AF_INET;
    seve.sin_port = htons(sport);
    inet_pton(AF_INET,sip,&seve.sin_addr.s_addr);

    int ret = connect(cfd,(struct sockaddr*)&seve,sizeof(seve));
    if(ret == -1)
        printf("error connect\n");
    //创建线程 心跳包 初始化锁
    pthread_mutex_init(&mtx_t,NULL);
    pthread_t tid;
    struct pdata pdata;
    pdata.cfd = cfd;
    pdata.seve = seve;
    pthread_create(&tid,NULL,pthread,(void*)&pdata);
    pthread_detach(tid);
    return cfd;
}

int senddata(int fd,char*data,int len)
{
    int packlen = len+4;
    char* package = (char*)malloc(packlen);
    memset(package,0,packlen);
    char* tp = package;
    char* tl = (char*)&len;
    int i=0; 
    for(i=0;i<4;i++)
    {
        tp[i]=tl[i];
    }
    for(i=0;i<len;i++)
    {
        tp[i+4] = *data++;
    }
    int ret = send(fd,package,packlen,0);
    if(ret == -1)
    {
        return -1;
    }
    return 0;
}


int recvdata(int fd,char**buf)
{
    //先读一个包头
    char clen[4]={0};
    int ret =recv(fd,clen,4,0);
    if(ret == -1)
    {
        printf("recv error\n");
        close(fd);
        return 0;
    }
    int len = *(int*)clen;
    int i=0;
    char* tbuf =(char*)malloc(len);
    char* temp = tbuf;
    memset(tbuf,0,len);
    for(i=0;i<len;i++)
    {
        ret = recv(fd,temp,1,0); 
        if(ret == -1)
        {
            printf("recv error\n");
            close(fd);
            return 0;
        }

        temp++;
    }
    *buf = tbuf;
    return len; 
}

void cliClose(int cfd)
{
    close(cfd);
}

#if 0
int main(int argc,char*argv[])
{
    int cfd = initcli("127.0.0.1",8080);
    char buf[1024] ={0} ;
    while(1)
    {
        memset(buf,0,1024);
        scanf("%s",buf);
        char* buf2= NULL;
        int len = strlen(buf)+1;
        senddata(cfd,buf,len);
        recvdata(cfd,&buf2);
        printf("%s\n",buf);
    }
    cliClose(cfd);
    return 0;
}
#endif


