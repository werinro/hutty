#include "channelcontexthandler.h"	


wlr::SocketChannelContextHandler::SocketChannelContextHandler(wlr::SocketChannel* socketChannel)
	: SocketChannel(NULL)
	, m_sc(socketChannel)
{
	this->m_f = wlr::SocketChannel::s_closed;
}


int wlr::SocketChannelContextHandler::read(wlr::ByteBuf *buf, int len) throw (wlr::SocketException)
{ return this->m_sc->read(buf, len);}

int wlr::SocketChannelContextHandler::write(wlr::ByteBuf &buf, int len) throw (wlr::SocketException)
{
	// 如果当前不是出栈并且当前不在出栈处理器中写出, 则从链尾开始调用
	wlr::ByteBuf *buffer = new wlr::ByteBuf(buf, len);
	wlr::ChannelHandlerContext* chc = this->currentContext();
	if (!this->inOutbound() || !chc)
		if (this->m_sc->channelPipeline())
            chc = this->m_sc->channelPipeline()->tailHandlerContext();

	int size = 0;
	if (chc) 
	{
		size = buffer->readable();
		chc->invokeChannelWrite(buffer);
	}
    delete buffer;
    return size;
}

int wlr::SocketChannelContextHandler::writeFlush(wlr::ByteBuf &buf, int len) throw (wlr::SocketException)
{
	this->flush();
	len = this->m_sc->write(buf, len);
	this->flush();
	return len;
}

bool wlr::SocketChannelContextHandler::tryRead()
{ return this->m_sc->tryRead(); }

bool wlr::SocketChannelContextHandler::tryWrite()
{ return this->m_sc->tryWrite(); }

bool wlr::SocketChannelContextHandler::isClose()
{ return this->m_sc->isClose(); }

void wlr::SocketChannelContextHandler::connect(std::string host, int port)
{ this->m_sc->connect(host, port); }

void wlr::SocketChannelContextHandler::flush()
{ this->m_sc->flush(); }

void wlr::SocketChannelContextHandler::close()
{ this->m_sc->close(); }

bool wlr::SocketChannelContextHandler::inOutbound()
{ return this->m_in_outbound.get(); }

void wlr::SocketChannelContextHandler::setInOutbound(bool value)
{ this->m_in_outbound.set(value); }

wlr::ChannelHandlerContext* wlr::SocketChannelContextHandler::currentContext()
{ return this->m_current_context.get(); }

void wlr::SocketChannelContextHandler::setCurrentContext(wlr::ChannelHandlerContext* dchc)
{ this->m_current_context.set(dchc); }

