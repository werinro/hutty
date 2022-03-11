#ifndef __WLR__HANDLER__H
#define __WLR__HANDLER__H


#include "../io/buf.h"
#include "../io/channel.h"
#include "../schedule/eventloopgroup.h"

namespace wlr
{


class ChannelHandlerContext;
class ChannelHandler
{
public:
	virtual ~ChannelHandler() { LOG_DEBUG("~ChannelHandler(%d)\n", this); }

	/**
	*	Channel handler function, called when the read or write is complete.
	*	@Param chc	current handler context
	*	@Param buf	data buf to be recv or send
	*/
	virtual void handler(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) = 0;

	/**
	*	An exception occurred while processing
	*	@Param	chc current handler context
	*	@Param	exception
	*/
	virtual void exceptioned(wlr::ChannelHandlerContext* chc, wlr::Exception* ex) = 0;

	/**
	*	When establishing a socket connection is called
	*	@Param  chc current handler context
	*/	
	virtual void connected(wlr::ChannelHandlerContext* chc) = 0;

	/**
	*	Called when a channel is registered with a selector
	*   @Param  chc current handler context
	*/ 
    virtual void registered(wlr::ChannelHandlerContext* chc) = 0;

	/**
	*	Called when the channel connection is closed
	*   @Param  chc current handler context
	*/ 
	virtual void closed(wlr::ChannelHandlerContext* chc) = 0;

	/**
	*	Called when channel related resources are destroyed
	*	@Param  chc current handler context
	*/ 
    virtual void destroyed(wlr::ChannelHandlerContext* chc) = 0;

	virtual void channelRead(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) = 0;
	virtual void channelWrite(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) = 0;

	/**
	*	Whether the handler is read
	*/	
	virtual bool inbound() = 0;

	/**
	*	Whether the handler is write
	*/ 
	virtual bool outbound() = 0;
};


class InboundChannelHandler : public ChannelHandler
{
public:
	virtual void handler(wlr::ChannelHandlerContext*, wlr::ByteBuf*) {}
    virtual void exceptioned(wlr::ChannelHandlerContext*, wlr::Exception*) {}
    virtual void connected(wlr::ChannelHandlerContext*) {}
    virtual void registered(wlr::ChannelHandlerContext*) {}
    virtual void closed(wlr::ChannelHandlerContext*) {}
    virtual void destroyed(wlr::ChannelHandlerContext*) {}
	virtual void channelWrite(wlr::ChannelHandlerContext*, wlr::ByteBuf*) {}
	virtual void channelRead(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) = 0;
	virtual bool inbound() {return true;}
	virtual bool outbound() {return false;}
};


class OutboundChannelHandler : public ChannelHandler
{
public:
	virtual void handler(wlr::ChannelHandlerContext*, wlr::ByteBuf*) {}
    virtual void exceptioned(wlr::ChannelHandlerContext*, wlr::Exception*) {}
    virtual void connected(wlr::ChannelHandlerContext*) {}
    virtual void registered(wlr::ChannelHandlerContext*) {}
    virtual void closed(wlr::ChannelHandlerContext*) {}
    virtual void destroyed(wlr::ChannelHandlerContext*) {}
	virtual void channelRead(wlr::ChannelHandlerContext*, wlr::ByteBuf*) {}
    virtual void channelWrite(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) = 0;
	virtual bool inbound() {return false;}
	virtual bool outbound() {return true;}
};


class ThreadPool;
class ChannelHandlerContext
{
public:
	ChannelHandlerContext(wlr::ChannelHandler* channel_handler, std::string name = "");

	std::string name();
	void setName(std::string name);
	wlr::ChannelHandler* channelHandler();
	void setChannelHandler(wlr::ChannelHandler* channel_handler);
	virtual wlr::SocketChannel* socketChannel() = 0;
	virtual wlr::EventLoopGroup* eventLoopGroup() = 0;
	virtual void invokeConnected() = 0;
    virtual void invokeRegistered() = 0;
    virtual void invokeChannelRead(wlr::ByteBuf* buf) = 0;
	virtual void invokeClosed() = 0;
    virtual void invokeDestroyed() = 0;
    virtual void invokeChannelWrite(wlr::ByteBuf* buf) = 0;
	virtual void invokeHandler(wlr::ByteBuf* buf) = 0;
    virtual void invokeExceptioned(wlr::Exception* ex) = 0;
	virtual ~ChannelHandlerContext();

protected:
	std::string m_name;
	wlr::ChannelHandler* m_handler = NULL;
};


class XHandler : public ChannelHandler
{
public:
	inline ChannelHandler* getInstance()
	{
		wlr::XHandler *x = new wlr::XHandler();
		x->inbound();
		return x;
	}
public:
    virtual void handler(wlr::ChannelHandlerContext*, wlr::ByteBuf*) {}
    virtual void exceptioned(wlr::ChannelHandlerContext*, wlr::Exception*) {}
    virtual void connected(wlr::ChannelHandlerContext*) {}
    virtual void registered(wlr::ChannelHandlerContext*) {}
    virtual void closed(wlr::ChannelHandlerContext*) {}
    virtual void destroyed(wlr::ChannelHandlerContext*) {}
	virtual void channelRead(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) {}
	virtual void channelWrite(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) {}
	virtual bool inbound() {return true;}
	virtual bool outbound() {return true;}
};


}

#endif
