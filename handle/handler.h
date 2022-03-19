#ifndef __WLR__HANDLER__H
#define __WLR__HANDLER__H


#include "../io/buf.h"
#include "../io/channel.h"
#include "../schedule/eventloopgroup.h"

namespace wlr
{



class IHandler
{
public:
	virtual ~IHandler() = default;
};



class ChannelHandlerContext;
class ChannelHandler : public virtual IHandler
{
public:
	virtual ~ChannelHandler() override { LOG_DEBUG("~ChannelHandler(%d)\n", this); }

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

};



class ChannelHandlerContext
{
public:
	ChannelHandlerContext(wlr::IHandler* handler, std::string name = "");

	std::string name();
	void setName(std::string name);
	wlr::IHandler* handler();
	void setChannelHandler(wlr::IHandler* handler);
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
	wlr::IHandler* m_handler = NULL;
};


}

#endif
