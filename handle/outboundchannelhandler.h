#ifndef __WLR__OUTBOUNDCHANNELHANDLER__H
#define __WLR__OUTBOUNDCHANNELHANDLER__H


#include "handler.h"


namespace wlr {


class IChannelOutbound : public virtual IHandler
{
public:
	/**
	*	channel write handler
	*	@param chc channel handler context
	*	@param buf channel write buf
	*/
	virtual void channelWrite(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) = 0;
};



class OutboundChannelHandler : public IChannelOutbound, public ChannelHandler
{
public:
	virtual ~OutboundChannelHandler() {}
    virtual void handler(wlr::ChannelHandlerContext*, wlr::ByteBuf*) {}
    virtual void exceptioned(wlr::ChannelHandlerContext*, wlr::Exception*) {}
    virtual void connected(wlr::ChannelHandlerContext*) {}
    virtual void registered(wlr::ChannelHandlerContext*) {}
    virtual void closed(wlr::ChannelHandlerContext*) {}
    virtual void destroyed(wlr::ChannelHandlerContext*) {}
};


}

#endif
