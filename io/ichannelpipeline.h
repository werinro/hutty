#ifndef __WLR__ICHANNELPIPELINE__H
#define __WLR__ICHANNELPIPELINE__H

#pragma once


#include <string>
#include "../utils/exception.h"
#include "../schedule/eventloopgroup.h"


namespace wlr
{


class ChannelHandler;
class ChannelHandlerContext;
class SocketChannel;
class ChannelPipeline
{
public:
	explicit ChannelPipeline(wlr::SocketChannel* socket_channel) : m_sc(socket_channel) {}
	virtual ~ChannelPipeline() { printf("~ChannelPipeline()\n"); }
	virtual ChannelPipeline* addFirst(wlr::ChannelHandler* handler, std::string name = "") = 0;
	virtual ChannelPipeline* addLast(wlr::ChannelHandler* handler, std::string name = "") = 0;
	virtual wlr::ChannelHandlerContext* handlerContext(std::string name) = 0;
	virtual wlr::ChannelHandlerContext* headHandlerContext() = 0;	
	virtual wlr::ChannelHandlerContext* tailHandlerContext() = 0;	
	virtual void fireConnected() = 0;
    virtual void fireRegistered() = 0;
    virtual void fireChannelRead() = 0;
    virtual void fireClosed() = 0;
    virtual void fireDestroyed() = 0;
    virtual void fireChannelWrite() = 0;
    virtual void fireHandler() = 0;
    virtual void fireExceptioned(wlr::Exception* ex) = 0;
	virtual void fireEventLoopGroup(wlr::EventLoopGroup* elg) = 0;
protected:
	wlr::SocketChannel* m_sc;
};



class IChannelPipeline
{
public:
	virtual ~IChannelPipeline() {}
	virtual wlr::ChannelPipeline* channelPipeline() { return NULL; }
};


}

#endif
