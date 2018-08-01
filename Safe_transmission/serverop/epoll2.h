#pragma once
#include<iostream>
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
#include"serverop.h"
#define EVENTS 1024
class ServerInfo;
class Epoll;
typedef void*(*epollrun)(Epoll*);
class Epoll
{
public:
	Epoll(){};
	~Epoll(){};
	//初始化  eodata(int cfd,int epfd)
	void initepoll(int port,char* ip,epollrun epdata);
	//接收数据
	int recvdata(int fd,char**buf,int epfd); 
	//发送数据
	int senddata(int fd,char*data,int len,int epfd);
public:
	int fd;
	int epfd;
	epollrun run;
	ServerInfo* sinfo;
private:
	
};
