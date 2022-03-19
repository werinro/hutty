#ifndef __WLR__MICH__H
#define __WLR__MICH__H


#include "../../handle/messagehandler.h"
#include "../../handle/initializerchannelhandler.h"


namespace wlr
{


class StringMessageHandler : public MessageHandler
{
public:
	virtual void handler(std::list<std::string> str_list);
	virtual ~StringMessageHandler() = default;
};


class MessageInitializerChannelHandler : public InitializerChannelHandler
{
public:
	virtual void initChannel(wlr::SocketChannel* sc);
};

}

#endif
