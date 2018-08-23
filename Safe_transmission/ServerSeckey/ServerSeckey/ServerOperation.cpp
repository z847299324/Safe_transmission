#include "ServerOperation.h"
#include <iostream>
#include "RequestFactory.h"
#include "RespondFactory.h"
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/hmac.h>
#include <unistd.h>
#include <signal.h>
using namespace std;

// 静态变量初始化
bool ServerOperation::m_stop = false;

ServerOperation::ServerOperation(ServerInfo * info)
{
	memcpy(&m_info, info, sizeof(ServerInfo));
}

ServerOperation::~ServerOperation()
{
}


void ServerOperation::startWork()
{
	// 0. 注册新号捕捉
	struct sigaction act;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	act.sa_handler = catchSignal;
	sigaction(SIGUSR1, &act, NULL);

	// 1. 创建/绑定/监听连接
	m_server.setListen(m_info.sPort);
	// 2. 等待连接请求
	while (1)
	{
		if (m_stop)
		{
			break;
		}
		// 接受连接请求
		TcpSocket* socket = m_server.acceptConn(5);
		if (socket == NULL)
		{
			cout << "accept 失败, 重新尝试..." << endl;
			continue;
		}
		// 创建子线程, 处理通信流程
		pthread_t thid;
		pthread_create(&thid, NULL, working, this);
		// 线程分离
		pthread_detach(thid);
		// 套接字对象和线程ID放入容器中
		m_listSocket.insert(make_pair(thid, socket));
	}
	cout << "程序即将退出..." << endl;
}

// 
int ServerOperation::secKeyAgree(RequestMsg * reqmsg, char ** outData, int & outLen)
{
	// 0. 验证消息认证码
	char seckey[1024];
	unsigned char md[MD5_DIGEST_LENGTH];
	char  md1[MD5_DIGEST_LENGTH * 2 + 1];
	unsigned int len;
	sprintf(seckey, "#%s@%s#", reqmsg->clientId, m_info.serverID);
	HMAC(EVP_md5(), seckey, strlen(seckey), (unsigned char*)reqmsg->r1, 
		strlen(reqmsg->r1), md, &len);
	
	// 将二进制的散列值 -> 16进制数字串格式
	for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
	{
		sprintf(&md1[i * 2], "%02x", md[i]);
	}
	if (strcmp(reqmsg->authCode, md1) != 0)
	{
		cout << "消息认证不一致..." << endl;
		return - 1;
	}

	// 1. 客户端解码之后的结构体
	// 服务器生成随机数r2 + r1
	RespondMsg resMsg;
	getRandString(sizeof(resMsg.r2), resMsg.r2);
	sprintf(seckey, "%s%s", reqmsg->r1, resMsg.r2);
	unsigned char tmpBuf[SHA_DIGEST_LENGTH];
	char tmpBuf1[SHA_DIGEST_LENGTH * 2 + 1];
	SHA1((unsigned char*)seckey, strlen(seckey), tmpBuf);
	// 将二进制的散列值 -> 16进制数字串格式
	for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
	{
		sprintf(&tmpBuf1[i * 2], "%02x", tmpBuf[i]);
	}
	cout << "服务器生成的秘钥: " << tmpBuf1 << endl;

	// 2. 组织服务器数据, 发送给客户端
	strcpy(resMsg.clientId, reqmsg->clientId);
	resMsg.rv = 0;	// 成功0, 失败-1
	resMsg.seckeyid = 100; // 秘钥的编号, 从数据库中读的
	strcpy(resMsg.serverId, m_info.serverID);
	// 3. 数据序列化
	CodecFactory* factory = new RespondFactory(&resMsg);
	Codec* codec = factory->createCodec();
	codec->msgEncode(outData, outLen);

	return 0;
}

void ServerOperation::catchSignal(int num)
{
	cout << "catch signal: " << num << endl;
	m_stop = true;
}

void ServerOperation::getRandString(int len, char * randBuf)
{
	srand(time(NULL));
	// 要求有0-9, a-z, A-Z, 特殊字符~!@#$%^&*()_+
	char sz[] = "~!@#$%^&*()_+";
	for (int i = 0; i < len - 1; ++i)
	{
		int flag = rand() % 4;	// 某一种格式
		switch (flag)
		{
		case 0:
			randBuf[i] = rand() % 10 + '0';
			break;
		case 1:
			randBuf[i] = rand() % 26 + 'a';
			break;
		case 2:
			randBuf[i] = rand() % 26 + 'A';
			break;
		case 3:
			randBuf[i] = sz[rand() % strlen(sz)];
			break;
		default:
			break;
		}
	}
	randBuf[len - 1] = '\0';
}

void * working(void * arg)
{
	usleep(1);
	// 1. 接收客户端发送的数据
	int recvLen;
	char* recvBuf = NULL;
	ServerOperation* sop = static_cast<ServerOperation*>(arg);
	pthread_t threadID = pthread_self();

	TcpSocket* socket = sop->m_listSocket[threadID];
	cout << "准备接收客户端数据..." << endl;
	socket->recvMsg(&recvBuf, recvLen);
	// 2. 需要解码收到的数据 -> RequestMsg
	CodecFactory* factory = new RequestFactory();
	Codec* codec = factory->createCodec();
	RequestMsg* reqmsg = static_cast<RequestMsg*>(codec->msgDecode(recvBuf, recvLen));
	char* outData;
	int outLen;
	
	switch (reqmsg->cmdType)
	{
	case RequestCodec::NewOrUpdate:
		// 秘钥协商
		sop->secKeyAgree(reqmsg, &outData, outLen);
		break;
	case RequestCodec::Check:
		break;
	case RequestCodec::Revoke:
		break;
	case RequestCodec::View:
		break;
	default:
		break;
	}
	socket->sendMsg(outData, outLen);

	// 关闭套接字连接
	socket->disConnect();
	auto it = sop->m_listSocket.find(threadID);
	sop->m_listSocket.erase(it);
	// 释放资源
	delete socket;

	return NULL;
}
