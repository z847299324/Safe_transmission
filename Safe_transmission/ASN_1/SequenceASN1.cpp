#include "SequenceASN1.h"

SequenceASN1::SequenceASN1()
{
}
//编码整形的头
int SequenceASN1::writeHeadNode(int iValue)
{
	DER_ItAsn1_WriteInteger(iValue, &m_header);
	m_next = m_header;
	return 0;
}
//编码字符串的头
int SequenceASN1::writeHeadNode(char * sValue, int len)
{
	//ITCAST_ANYBUF*tmp=NULL;
	DER_ITCAST_String_To_AnyBuf(&m_temp,(unsigned char*)sValue,len);
	DER_ItAsn1_WriteBitString(m_temp,&m_header);
	if (m_temp != NULL)
		free(m_temp);
	m_next = m_header;
	return 0;
}
//编码后继节点int
int SequenceASN1::writeNextNode(int iValue)
{
	DER_ItAsn1_WriteInteger(iValue, &m_next->next);
	m_next = m_next->next;
	return 0;
}
//编码后继节点char*
int SequenceASN1::writeNextNode(char * sValue, int len)
{
	DER_ITCAST_String_To_AnyBuf(&m_temp, (unsigned char*)sValue, len);
	DER_ItAsn1_WriteBitString(m_temp, &m_next->next);
	DER_ITCAST_FreeQueue(m_temp);
	m_next = m_next->next;
	return 0;
}
//读头int
int SequenceASN1::readHeadNode(int & iValue)
{
	DER_ItAsn1_ReadInteger(m_header, (ITCAST_UINT32*)&iValue);
	m_next = m_header->next;
	return 0;
}
//读头char*
int SequenceASN1::readHeadNode(char * sValue)
{
	DER_ItAsn1_ReadBitString(m_header, &m_temp);
	memcpy(sValue, m_temp->pData, m_temp->dataLen);
	DER_ITCAST_FreeQueue(m_temp);
	m_next = m_header->next;
	return 0;
}
//读后继int
int SequenceASN1::readNextNode(int & iValue)
{
	DER_ItAsn1_ReadInteger(m_next, (ITCAST_UINT32*)&iValue);
	m_next = m_next->next;
	return 0;
}
//读后记char*
int SequenceASN1::readNextNode(char * sValue)
{
	DER_ItAsn1_ReadBitString(m_next, &m_temp);
	memcpy(sValue, m_temp->pData, m_temp->dataLen);
	DER_ITCAST_FreeQueue(m_temp);
	m_next = m_next->next;
	return 0;
}
//打包
int SequenceASN1::packSequence(char ** outData, int & outLen)
{
	DER_ItAsn1_WriteSequence(m_header, &m_temp);
	*outData = (char*)malloc(m_temp->dataLen);
	memcpy(*outData, m_temp->pData,m_temp->dataLen);
	outLen = m_temp->dataLen;
	DER_ITCAST_FreeQueue(m_temp);
	return 0;
}
//解包
int SequenceASN1::unpackSequence(char * inData, int inLen)
{
	DER_ITCAST_String_To_AnyBuf(&m_temp, (unsigned char*)inData, inLen);
	DER_ItAsn1_ReadSequence(m_temp, &m_header);
	DER_ITCAST_FreeQueue(m_temp);
	return 0;
}

void SequenceASN1::freeSequence(ITCAST_ANYBUF * node)
{
	while (node != NULL)
	{
		m_temp = node->next;
		DER_ITCAST_FreeQueue(node);
		node = m_temp;
	}
}
