#include <cstdio>
#include <sys/types.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ServerOperation.h"
#include <fcntl.h>
#include <iostream>
using namespace std;

void createDeamon();
int main()
{
	// 程序变成守护进程
	createDeamon();

	ServerInfo info;
	info.maxnode = 20;	// 秘钥协商服务器支持和20个客户端进行秘钥协商
	strcpy(info.serverID, "222");
	info.shmkey = ftok("/bin", 'w');
	info.sPort = 8989; // 服务器绑定的端口
	// 构造服务器操作对象
	ServerOperation serverOP(&info);
	// 服务器开始干活
	serverOP.startWork();

    printf("进程结束...!\n");
    return 0;
}

void createDeamon()
{
	// 1. 创建子进程
	pid_t pid = fork();
	if (pid > 0)
	{
		// 父进程
		// 2. 退出父进程
		exit(0);
	}
	// 3. 子进程提升为会话
	setsid();	// - 脱离终端
	// 4. 可选项 - 修改进程的工作目录
	chdir("/home");
	//		磁盘 /home/kevin/U盘/app
	//		在a目录下启动app ./app
	// 5. 可选项 - 修改掩码 - umask4.
	umask(0022);
	// 6. 可选项 - 文件描述符关闭/重定向(推荐) /dev/null
	int fd = open("/dev/null", O_RDWR);
	dup2(fd, 0);
	//dup2(fd, 1);
	dup2(fd, 2);
	// 7. 核心操作
	// 8. 管理守护进程的退出 - 脚本
}