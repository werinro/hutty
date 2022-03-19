#ifndef __WLR__MESSAGEHANDLER__H
#define __WLR__MESSAGEHANDLER__H


#include <string>
#include "bytetransformhandler.h"


namespace wlr
{


class MessageHandler 
	: public ByteTransformHandler<std::string> 
	, public ChannelHandler
{

	virtual void channelReadHandler(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* in_buf, std::list<std::string> *out_list) override;
    virtual void channelWriteHandler(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* out_buf, std::list<std::string> in_list) override;
public:

	virtual void handler(wlr::ChannelHandlerContext*, wlr::ByteBuf*) override;
    virtual void exceptioned(wlr::ChannelHandlerContext*, wlr::Exception*) override;
    virtual void connected(wlr::ChannelHandlerContext*) override;
    virtual void registered(wlr::ChannelHandlerContext*) override;
    virtual void closed(wlr::ChannelHandlerContext*) override;
    virtual void destroyed(wlr::ChannelHandlerContext*) override;

	virtual ~MessageHandler() override = default;
};



}



#endif
