#pragma once
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
//初始化  eodata(int cfd,int epfd)
void initepoll(int port,char* ip,void*(epdata)(int,int));
//接收数据
int recvdata(int fd,char**buf,int epfd); 
//发送数据
int senddata(int fd,char*data,int len);
