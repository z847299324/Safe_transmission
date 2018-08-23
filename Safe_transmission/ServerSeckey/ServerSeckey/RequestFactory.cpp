#include "RequestFactory.h"
#include "RequestCodec.h"
#include <iostream>
using namespace std;

RequestFactory::RequestFactory()
{
	m_flag = false;
}

RequestFactory::RequestFactory(RequestMsg* msg)
{
	m_flag = true;
	m_request = msg;
}


RequestFactory::~RequestFactory()
{
}

Codec * RequestFactory::createCodec()
{
	Codec* codec = NULL;
	if (m_flag)
	{
		codec = new RequestCodec(m_request);
	}
	else
	{
		codec = new RequestCodec();
	}
	m_ptr.reset(codec);

	return codec;
}
