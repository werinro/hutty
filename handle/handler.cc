#include "handler.h"


wlr::ChannelHandlerContext::ChannelHandlerContext(wlr::ChannelHandler* channel_handler, std::string name)
    : m_handler(channel_handler)
	, m_name(name)
{}

std::string wlr::ChannelHandlerContext::name()
{ return this->m_name; }

void wlr::ChannelHandlerContext::setName(std::string name)
{ this->m_name = name; }

wlr::ChannelHandler* wlr::ChannelHandlerContext::channelHandler()
{ return this->m_handler; }

void wlr::ChannelHandlerContext::setChannelHandler(wlr::ChannelHandler* channel_handler)
{ this->m_handler = channel_handler; }

wlr::ChannelHandlerContext::~ChannelHandlerContext()
{ 
	if (this->m_handler) 
	{ 
		delete this->m_handler; 
		this->m_handler = NULL; 
	}
}
