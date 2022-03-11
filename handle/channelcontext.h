#ifndef	__WLR__CHANNELCONTEXT__H
#define __WLR__CHANNELCONTEXT__H 


#include "channelcontexthandler.h"
#include "../schedule/eventloopgroup.h"


namespace wlr
{


class DefaultChannelHandlerContext : public ChannelHandlerContext
{
public:
    DefaultChannelHandlerContext(wlr::SocketChannel* socket_channel, wlr::ChannelHandler* channel_handler, std::string name = "default");

    wlr::ChannelHandler* channelHandler();
    void setChannelHandler(wlr::ChannelHandler* channel_handler);
    void setEventLoopGroup(wlr::EventLoopGroup* event_loop_group);

	wlr::DefaultChannelHandlerContext* prev();
	wlr::DefaultChannelHandlerContext* next();
    wlr::DefaultChannelHandlerContext* removePrev();
    wlr::DefaultChannelHandlerContext* insertPrev(wlr::DefaultChannelHandlerContext* prev);
    wlr::DefaultChannelHandlerContext* removeNext();
    wlr::DefaultChannelHandlerContext* insertNext(wlr::DefaultChannelHandlerContext* next);

	virtual wlr::SocketChannel* socketChannel() override;
	virtual wlr::EventLoopGroup* eventLoopGroup() override;
    virtual void invokeConnected() override;
    virtual void invokeRegistered() override;
    virtual void invokeChannelRead(wlr::ByteBuf* buf) override;
    virtual void invokeClosed() override;
    virtual void invokeDestroyed() override;
    virtual void invokeChannelWrite(wlr::ByteBuf* buf) override;
    virtual void invokeHandler(wlr::ByteBuf* buf) override;
    virtual void invokeExceptioned(wlr::Exception* ex) override;
    virtual ~DefaultChannelHandlerContext() override;

protected:
	wlr::EventLoopGroup* m_elg = NULL;
	wlr::SocketChannelContextHandler* m_scc_handler;
    wlr::DefaultChannelHandlerContext* m_prev = NULL;
    wlr::DefaultChannelHandlerContext* m_next = NULL;
};


class HeadChannelHandlerContext : public DefaultChannelHandlerContext
{
public:
	HeadChannelHandlerContext(wlr::SocketChannel* socket_channel, wlr::ChannelHandler* channel_handler = NULL);
	virtual void invokeChannelRead(wlr::ByteBuf* buf) override;
	virtual void invokeChannelWrite(wlr::ByteBuf* buf) override;
};


class TailChannelHandlerContext : public DefaultChannelHandlerContext
{
public:
	TailChannelHandlerContext(wlr::SocketChannel* socket_channel, wlr::ChannelHandler* channel_handler = NULL);
	virtual void invokeChannelRead(wlr::ByteBuf* buf) override;
	virtual void invokeChannelWrite(wlr::ByteBuf* buf) override;
};


}


#endif
