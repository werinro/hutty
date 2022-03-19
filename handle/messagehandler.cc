#include "messagehandler.h"


void wlr::MessageHandler::handler(wlr::ChannelHandlerContext* chc, wlr::ByteBuf*)
{
}

void wlr::MessageHandler::exceptioned(wlr::ChannelHandlerContext*, wlr::Exception*)
{
}

void wlr::MessageHandler::connected(wlr::ChannelHandlerContext*)
{
}

void wlr::MessageHandler::registered(wlr::ChannelHandlerContext*)
{
}

void wlr::MessageHandler::closed(wlr::ChannelHandlerContext*)
{
}

void wlr::MessageHandler::destroyed(wlr::ChannelHandlerContext*)
{
}


void wlr::MessageHandler::channelReadHandler(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf, std::list<std::string> *out_list)
{
    int size = buf->readable();
    char buffer[size];
    int len = buf->readBytes(buffer, size);

    out_list->push_back(std::string(buffer, len));
    wlr::SocketChannel* sc = chc->socketChannel();
    LOG_DEBUG("channel %d readable = %d read len = %d\n", sc->channelId().id(), size, len);
}

void wlr::MessageHandler::channelWriteHandler(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf, std::list<std::string> in_list)
{
    for (std::string str : in_list)
        buf->writeBytes(str.c_str(), 0, str.length());
}

