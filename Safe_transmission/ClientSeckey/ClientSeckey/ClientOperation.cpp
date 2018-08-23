#include "ClientOperation.h"
#include <string.h>
#include <iostream>
#include "RequestCodec.h"
#include <time.h>
#include <openssl/hmac.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include "RequestFactory.h"
#include "RespondCodec.h"
#include "RespondFactory.h"
// using namespace std;

ClientOperation::ClientOperation(ClientInfo * info)
{
	memcpy(&m_info, info, sizeof(ClientInfo));
	// 创建共享内存对象
	m_shm = new SecKeyShm(m_info.shmKey, m_info.maxNode);
}

ClientOperation::~ClientOperation()
{
	// 根据实际情况考虑是否要删除
	m_shm->delShm();
	delete m_shm;
}

int ClientOperation::secKeyAgree()
{
	// 1. 创建一个 RequestMsg对象, 并初始化
	RequestMsg reqMsg;
	strcpy(reqMsg.clientId, m_info.clinetID);
	strcpy(reqMsg.serverId, m_info.serverID);
	reqMsg.cmdType = RequestCodec::NewOrUpdate;
	getRandString(sizeof(reqMsg.r1), reqMsg.r1);
	// 消息认证码 = (原始数据(r1) + 秘钥)*哈希运算
	char seckey[1024];
	unsigned char md[MD5_DIGEST_LENGTH];
	char  md1[MD5_DIGEST_LENGTH*2+1];
	unsigned int len;
	sprintf(seckey, "#%s@%s#", reqMsg.clientId, reqMsg.serverId);
	HMAC_CTX* ctx = HMAC_CTX_new();
	HMAC_Init(ctx, seckey, strlen(seckey), EVP_md5());
	HMAC_Update(ctx, (unsigned char*)reqMsg.r1, strlen(reqMsg.r1));
	HMAC_Final(ctx, md, &len);
	// 将二进制的散列值 -> 16进制数字串格式
	for (int i = 0; i < MD5_DIGEST_LENGTH; ++i)
	{
		sprintf(&md1[i * 2], "%02x", md[i]);
	}
	strcpy(reqMsg.authCode, md1);

	// 2. 将要发送的数据序列化 - 得到一个字符串
	// 2.1 创建工厂对象
	CodecFactory* factory = new RequestFactory(&reqMsg);
	// 2.2 创建编解码对象
	Codec* codec = factory->createCodec();
	// 2.3 编码
	char* outData = NULL;
	int outLen = 0;
	codec->msgEncode(&outData, outLen);

	// 3. 发送数据
	// 3.1 连接服务器
	m_socket.connectToHost(m_info.serverIP, m_info.serverPort);
	// 3.2 发送数据
	m_socket.sendMsg(outData, outLen);
	// outData需要释放
	free(outData);
	
	// 4. 接收数据 - 服务器回发的数据 - > 字符串
	char* recvBuf = NULL;
	m_socket.recvMsg(&recvBuf, outLen);

	// 5. 将接收的数据解码
	// 5.1 释放对象内存
	delete factory;
	// delete codec;
	factory = new RespondFactory();
	codec = factory->createCodec();
	RespondMsg *resMsg = (RespondMsg*)codec->msgDecode(recvBuf, outLen);
	// 释放recvBuf
	m_socket.freeMemory(&recvBuf);

	// 6. 判断服务器时候将请求处理成功了
	if (resMsg->rv == -1)
	{
		cout << "服务器秘钥协商失败..." << endl;
		return -1;
	}
	// 7. 成功了, 根据服务器给到的数据r2生成秘钥
	// r1+r2, 哈希运算->秘钥
	sprintf(seckey, "%s%s", reqMsg.r1, resMsg->r2);
	unsigned char tmpBuf[SHA_DIGEST_LENGTH];
	char tmpBuf1[SHA_DIGEST_LENGTH * 2 + 1];
	SHA1((unsigned char*)seckey, strlen(seckey), tmpBuf);
	// 将二进制的散列值 -> 16进制数字串格式
	for (int i = 0; i < SHA_DIGEST_LENGTH; ++i)
	{
		sprintf(&tmpBuf1[i * 2], "%02x", tmpBuf[i]);
	}
	cout << "客户端生成的秘钥: " << tmpBuf1 << endl;

	// 8. 秘钥写共享内存
	NodeSHMInfo info;
	info.status = 0;	// 0: 可用, -1:不可用
	info.seckeyID = resMsg->seckeyid;
	strcpy(info.clientID, m_info.clinetID);
	strcpy(info.serverID, m_info.serverID);
	strcpy(info.seckey, tmpBuf1);
	m_shm->shmWrite(&info);

	// 9. 内存释放
	delete factory;
	// delete codec;

	// 10. 关闭通信的套接字
	m_socket.disConnect();

	return 0;
}

void ClientOperation::getRandString(int len, char * randBuf)
{
	srand(time(NULL));
	// 要求有0-9, a-z, A-Z, 特殊字符~!@#$%^&*()_+
	char sz[] = "~!@#$%^&*()_+";
	for (int i = 0; i < len-1; ++i)
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
