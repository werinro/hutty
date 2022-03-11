#include "initializerchannelhandler.h"


void wlr::InitializerChannelHandler::init(wlr::SocketChannel* socket_channel, wlr::EventLoopGroup* event_loop_group)
{
	wlr::ChannelPipeline* cp = socket_channel->channelPipeline();
	this->initChannel(socket_channel);
	cp->fireEventLoopGroup(event_loop_group);
}

void wlr::InitializerChannelHandler::initChannel(wlr::SocketChannel*) {}

