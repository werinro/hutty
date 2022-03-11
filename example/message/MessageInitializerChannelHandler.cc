#include "MessageInitializerChannelHandler.h"
#include "../../structure/maps.h"


void wlr::StringMessageHandler::channelReadMessage(std::list<std::string>& in_list)
{
	wlr::foreach(in_list, [](std::string message, int index){
		LOG_INFO("recv client message = %s\n", message.c_str());
	});
}

void wlr::StringMessageHandler::channelWriteMessage(std::list<std::string> *out_list)
{
	out_list->push_back("The server has received the message\n");
}


void wlr::MessageInitializerChannelHandler::initChannel(wlr::SocketChannel* sc)
{
	wlr::ChannelPipeline* cp = sc->channelPipeline();
	cp->addFirst(new wlr::StringMessageHandler(), "message_handler");
}

