#pragma once
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
//初始化客户端
int initcli(char*sip,int sport);
//发送数据
int senddata(int fd,char*data,int len);
//接收数据
int recvdata(int fd,char**buf);
//关闭cfd
void cliClose(int cfd);
