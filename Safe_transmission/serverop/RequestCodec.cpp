//请求秘钥结构体编解码

#include "RequestCodec.h"
RequestCodec::RequestCodec()
{
}

RequestCodec::RequestCodec(RequestMsg * msg)
{
	//memcpy(&m_msg, msg, sizeof(RequestCodec));
	m_msg = *msg;
}

RequestCodec::~RequestCodec()
{
}
/*
int		cmdType;		// 报文类型
char	clientId[12];	// 客户端编号
char	authCode[65];	// 认证码
char	serverId[12];	// 服务器端编号
char	r1[64];			// 客户端随机数
*/

//编码
int RequestCodec::msgEncode(char ** outData, int & len)
{
	writeHeadNode(m_msg.cmdType);		//编码头部
	writeNextNode(m_msg.clientId, strlen(m_msg.clientId) + 1);	//编码节点
	writeNextNode(m_msg.authCode, strlen(m_msg.authCode) + 1);
	writeNextNode(m_msg.serverId, strlen(m_msg.serverId) + 1);
	writeNextNode(m_msg.r1, strlen(m_msg.r1) + 1);
	packSequence(outData, len);		//打包
	return 0;
}
//解码
void * RequestCodec::msgDecode(char * inData, int inLen)
{
	unpackSequence(inData, inLen);
	readHeadNode(m_msg.cmdType);//解码头部
	readNextNode(m_msg.clientId);//解码节点
	readNextNode(m_msg.authCode);
	readNextNode(m_msg.serverId);
	readNextNode(m_msg.r1);
	printf("%s\n", m_msg.clientId);
	return &this->m_msg;
}
