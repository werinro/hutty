#ifndef __WLR__MICH__H
#define __WLR__MICH__H


#include "../../handle/messagehandler.h"
#include "../../handle/initializerchannelhandler.h"


namespace wlr
{


class StringMessageHandler : public MessageHandler
{
public:
	virtual void channelReadMessage(std::list<std::string>& in_list);
    virtual void channelWriteMessage(std::list<std::string>* out_list);
};


class MessageInitializerChannelHandler : public InitializerChannelHandler
{
public:
	virtual void initChannel(wlr::SocketChannel* sc);
};

}

#endif
