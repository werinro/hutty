#ifndef __WLR__INBOUNDCHANNELHANDLER__H
#define __WLR__INBOUNDCHANNELHANDLER__H


#include "handler.h"


namespace wlr {


class IChannelInbound : public virtual IHandler
{
public:
	/**
	*	channel read handler
	*	@param chc channel handler context
	*	@param buf channel read byte buf
	*/ 
    virtual void channelRead(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) = 0;
};



class InboundChannelHandler : public IChannelInbound, public ChannelHandler
{
public:
	virtual ~InboundChannelHandler() {}
    virtual void handler(wlr::ChannelHandlerContext*, wlr::ByteBuf*) {}
    virtual void exceptioned(wlr::ChannelHandlerContext*, wlr::Exception*) {}
    virtual void connected(wlr::ChannelHandlerContext*) {}
    virtual void registered(wlr::ChannelHandlerContext*) {}
    virtual void closed(wlr::ChannelHandlerContext*) {}
    virtual void destroyed(wlr::ChannelHandlerContext*) {}
};


}

#endif
