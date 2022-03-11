#ifndef __WLR__MESSAGEHANDLER__H
#define __WLR__MESSAGEHANDLER__H


#include <list>
#include "handler.h"


namespace wlr
{


class MessageHandler : public ChannelHandler
{
private:
	virtual void handler(wlr::ChannelHandlerContext*, wlr::ByteBuf*) override;
	virtual void exceptioned(wlr::ChannelHandlerContext*, wlr::Exception*) override;
    virtual void connected(wlr::ChannelHandlerContext*) override;
    virtual void registered(wlr::ChannelHandlerContext*) override;
    virtual void closed(wlr::ChannelHandlerContext*) override;
    virtual void destroyed(wlr::ChannelHandlerContext*) override;
    virtual bool inbound() override {return true;}
    virtual bool outbound() override {return true;}

	virtual void channelRead(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) override;
	virtual void channelWrite(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) override;

public:
	virtual void channelReadMessage(std::list<std::string>&) {}
	virtual void channelWriteMessage(std::list<std::string>*) {}
};





}



#endif
