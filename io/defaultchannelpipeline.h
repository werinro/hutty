#ifndef __WLR__DEFAULTCHANNELPIPELINE__H
#define __WLR__DEFAULTCHANNELPIPELINE__H


#include "ichannelpipeline.h"
#include "../handle/channelcontext.h"



namespace wlr
{


class DefaultChannelPipeline : public ChannelPipeline
{
public:
	DefaultChannelPipeline(wlr::SocketChannel* socket_channel);
	virtual ~DefaultChannelPipeline() override;
	virtual wlr::ChannelPipeline* addFirst(wlr::IHandler* handler, std::string name) override;
	virtual wlr::ChannelPipeline* addLast(wlr::IHandler* handler, std::string name) override;
	virtual wlr::ChannelHandlerContext* handlerContext(std::string name) override;
	virtual wlr::ChannelHandlerContext* headHandlerContext() override;	
	virtual wlr::ChannelHandlerContext* tailHandlerContext() override;	
	
	virtual void fireConnected() override;
    virtual void fireRegistered() override;
    virtual void fireChannelRead() override;
    virtual void fireClosed() override;
    virtual void fireDestroyed() override;
    virtual void fireChannelWrite() override;
    virtual void fireHandler() override;
    virtual void fireExceptioned(wlr::Exception* ex) override;
	virtual void fireEventLoopGroup(wlr::EventLoopGroup* elp) override;
	virtual bool destroyed() override;

private:
	bool m_destroyed = 0;
	wlr::SocketChannelContextHandler* m_scc_handler;
	wlr::DefaultChannelHandlerContext* m_head;
	wlr::DefaultChannelHandlerContext* m_tail;
};



}

#endif
