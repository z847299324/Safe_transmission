//������Կ�ṹ������

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
int		cmdType;		// ��������
char	clientId[12];	// �ͻ��˱��
char	authCode[65];	// ��֤��
char	serverId[12];	// �������˱��
char	r1[64];			// �ͻ��������
*/

//����
int RequestCodec::msgEncode(char ** outData, int & len)
{
	writeHeadNode(m_msg.cmdType);		//����ͷ��
	writeNextNode(m_msg.clientId, strlen(m_msg.clientId) + 1);	//����ڵ�
	writeNextNode(m_msg.authCode, strlen(m_msg.authCode) + 1);
	writeNextNode(m_msg.serverId, strlen(m_msg.serverId) + 1);
	writeNextNode(m_msg.r1, strlen(m_msg.r1) + 1);
	packSequence(outData, len);		//���
	return 0;
}
//����
void * RequestCodec::msgDecode(char * inData, int inLen)
{
	unpackSequence(inData, inLen);
	readHeadNode(m_msg.cmdType);//����ͷ��
	readNextNode(m_msg.clientId);//����ڵ�
	readNextNode(m_msg.authCode);
	readNextNode(m_msg.serverId);
	readNextNode(m_msg.r1);
	printf("%s\n", m_msg.clientId);
	return &this->m_msg;
}
