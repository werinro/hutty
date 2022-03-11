#ifndef __WLR__INITIALIZERCHANNELHANDLER__H
#define __WLR__INITIALIZERCHANNELHANDLER__H


#include "../io/channel.h"
#include "../io/ichannelpipeline.h"
#include "channelcontext.h"


namespace wlr
{


class InitializerChannelHandler
{
public:
	void init(wlr::SocketChannel*, wlr::EventLoopGroup*);
	virtual void initChannel(wlr::SocketChannel*);
};


}

#endif
