#include "MessageInitializerChannelHandler.h"
#include "../../structure/maps.h"


void wlr::StringMessageHandler::handler(std::list<std::string> str_list) 
{
	wlr::foreach(str_list, [](std::string message, int index){
        LOG_INFO("recv client message = %s\n", message.c_str());
    });

	this->write("The server has received the message\n");
}



void wlr::MessageInitializerChannelHandler::initChannel(wlr::SocketChannel* sc)
{
	wlr::ChannelPipeline* cp = sc->channelPipeline();
	cp->addFirst(new wlr::StringMessageHandler(), "message_handler");
}

