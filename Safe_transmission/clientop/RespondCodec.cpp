#include "RespondCodec.h"

RespondCodec::RespondCodec()
{
}

RespondCodec::RespondCodec(RespondMsg * msg)
{
	//m_msg = *msg;
    memcpy(&m_msg,msg,sizeof(RespondMsg));
}

RespondCodec::~RespondCodec()
{
}

/*
int		rv;				// 返回值
char	clientId[12];	// 客户端编号
char	serverId[12];	// 服务器编号
char	r2[64];			// 服务器端随机数
int		seckeyid;		// 对称密钥编号    keysn
*/
//编码
int RespondCodec::msgEncode(char ** outData, int & len)
{
	writeHeadNode(m_msg.rv);//编码头
	writeNextNode(m_msg.clientId,strlen(m_msg.clientId)+1);//编码节点
	writeNextNode(m_msg.serverId,strlen(m_msg.serverId)+1);
	writeNextNode(m_msg.r2,strlen(m_msg.r2)+1);
	writeNextNode(m_msg.seckeyid);
	packSequence(outData,len);//打包
	return 0;
}
//解码
void * RespondCodec::msgDecode(char * inData, int inLen)
{
	unpackSequence(inData, inLen);//解包
	readHeadNode(m_msg.rv);
	readNextNode(m_msg.clientId);
	readNextNode(m_msg.serverId);
	readNextNode(m_msg.r2);
	readNextNode(m_msg.seckeyid);
	return &this->m_msg;
}
