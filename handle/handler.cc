#include "handler.h"


wlr::ChannelHandlerContext::ChannelHandlerContext(wlr::IHandler* handler, std::string name)
    : m_handler(handler)
	, m_name(name)
{
	// if (!handler) W_THROW(NullPointerException, "handler");
}

std::string wlr::ChannelHandlerContext::name()
{ return this->m_name; }

void wlr::ChannelHandlerContext::setName(std::string name)
{ this->m_name = name; }

wlr::IHandler* wlr::ChannelHandlerContext::handler()
{ return this->m_handler; }

void wlr::ChannelHandlerContext::setChannelHandler(wlr::IHandler* handler)
{ 
	if (!handler) W_THROW(NullPointerException, "handler");
	this->m_handler = handler;
}

wlr::ChannelHandlerContext::~ChannelHandlerContext()
{ 
	if (this->m_handler) 
	{ 
		delete this->m_handler; 
		this->m_handler = NULL; 
	}
}
