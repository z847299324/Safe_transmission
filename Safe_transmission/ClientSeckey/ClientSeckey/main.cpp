#include <cstdio>
#include <iostream>
#include "RequestCodec.h"
#include "ClientOperation.h"
#include <sys/types.h>
#include <sys/ipc.h>
using namespace std;

int usage();
int main()
{
	ClientInfo info;
	// 测试程序
	// 1. 桌面窗口程序 - 有设置界面
	// 2. 从配置文件中读数据
	info.maxNode = 1;	// 秘钥协商客户端存储一个秘钥
	info.serverPort = 8989;	// 连接的服务器绑定的端口
	info.shmKey = ftok("/home", 'x');
	strcpy(info.clinetID, "111");
	strcpy(info.serverID, "222");
	strcpy(info.serverIP, "127.0.0.1");
	ClientOperation client(&info);

	while (1)
	{
		int sel = usage();
		switch (sel)
		{
		case 0:
			cout << "程序退出..." << endl;
			return 0;
		case RequestCodec::NewOrUpdate:
			client.secKeyAgree();
			break;
		case RequestCodec::Check:
			client.secKeyCheck();
			break;
		case RequestCodec::Revoke:
			client.secKeyRevoke();
			break;
		case RequestCodec::View:
			client.secKeyView();
			break;
		default:
			break;
		}
	}
    return 0;
}

int usage()
{
	int nSel = -1;
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n  /*     1.密钥协商                                            */");
	printf("\n  /*     2.密钥校验                                            */");
	printf("\n  /*     3.密钥注销                                            */");
	printf("\n  /*     4.密钥查看                                            */");
	printf("\n  /*     0.退出系统                                            */");
	printf("\n  /*************************************************************/");
	printf("\n  /*************************************************************/");
	printf("\n\n  选择:");

	scanf("%d", &nSel);
	while (getchar() != '\n');

	return nSel;
}