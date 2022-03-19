#include "channelcontext.h"
#include "inboundchannelhandler.h"
#include "outboundchannelhandler.h"


wlr::DefaultChannelHandlerContext::DefaultChannelHandlerContext(wlr::SocketChannel* socket_channel, wlr::IHandler* handler, std::string name)
	: ChannelHandlerContext(handler, name)
{
	this->m_scc_handler = dynamic_cast<wlr::SocketChannelContextHandler*>(socket_channel);
}

void wlr::DefaultChannelHandlerContext::setEventLoopGroup(wlr::EventLoopGroup* event_loop_group)
{ this->m_elg = event_loop_group; }

wlr::DefaultChannelHandlerContext* wlr::DefaultChannelHandlerContext::prev()
{ return this->m_prev; }

wlr::DefaultChannelHandlerContext* wlr::DefaultChannelHandlerContext::next()
{ return this->m_next; }

#define W_ASSIGNMENT_CHC(field) \
	wlr::DefaultChannelHandlerContext* chc; \
	if (chc = field)	

wlr::DefaultChannelHandlerContext* wlr::DefaultChannelHandlerContext::removePrev()
{
	W_ASSIGNMENT_CHC(this->m_prev)
	{
		this->m_prev = this->m_prev->m_prev;
		if (this->m_prev) this->m_prev->m_next = this;
	}
	
	return chc;
}

wlr::DefaultChannelHandlerContext* wlr::DefaultChannelHandlerContext::insertPrev(wlr::DefaultChannelHandlerContext* prev)
{
	if (!prev) return NULL;
	W_ASSIGNMENT_CHC(this->m_prev)
	{
		this->m_prev->m_next = prev;
		prev->m_prev = this->m_prev;
	}
	this->m_prev = prev;
	prev->m_next = this;
	return chc;
}

wlr::DefaultChannelHandlerContext* wlr::DefaultChannelHandlerContext::removeNext()
{
	W_ASSIGNMENT_CHC(this->m_next)
    {
        this->m_next = this->m_next->m_next;
        if (this->m_next) this->m_next->m_prev = this;
    }

    return chc;
}

wlr::DefaultChannelHandlerContext* wlr::DefaultChannelHandlerContext::insertNext(wlr::DefaultChannelHandlerContext* next)
{
	if (!next) return NULL;
    W_ASSIGNMENT_CHC(this->m_next)
    {
        this->m_next->m_prev = next;
        next->m_next = this->m_next;
    } 
	this->m_next = next;
    next->m_prev = this;
    return chc;
}

wlr::SocketChannel* wlr::DefaultChannelHandlerContext::socketChannel()
{ return this->m_scc_handler; }

wlr::EventLoopGroup* wlr::DefaultChannelHandlerContext::eventLoopGroup()
{ return this->m_elg; }


#define W_DEFINITION0(func, invoke) \
void wlr::DefaultChannelHandlerContext::func() \
{ \
	wlr::ChannelHandler* ch = dynamic_cast<wlr::ChannelHandler*>(this->m_handler); \
	if (ch) ch->invoke(this); \
    if (this->m_next) this->m_next->func(); \
}
#define W_DEFINITION1(func, invoke, type) \
void wlr::DefaultChannelHandlerContext::func(type val) \
{ \
	wlr::ChannelHandler* ch = dynamic_cast<wlr::ChannelHandler*>(this->m_handler); \
    if (ch) ch->invoke(this, val); \
    if (this->m_next) this->m_next->func(val); \
}

W_DEFINITION0(invokeConnected, connected)	
W_DEFINITION0(invokeRegistered, registered)	

void wlr::DefaultChannelHandlerContext::invokeChannelRead(wlr::ByteBuf* buf)
{
	wlr::IChannelInbound* ici = dynamic_cast<wlr::IChannelInbound*>(this->m_handler);
	if (ici) ici->channelRead(this, buf);
	if (this->m_next) this->m_next->invokeChannelRead(buf);
}

W_DEFINITION0(invokeClosed, closed)
W_DEFINITION0(invokeDestroyed, destroyed)

void wlr::DefaultChannelHandlerContext::invokeChannelWrite(wlr::ByteBuf* buf)
{
	if (this->m_scc_handler) 
	{
		if (this->m_scc_handler->currentContext() == this) {
			this->m_prev->invokeChannelWrite(buf);
			return;
		} else
			this->m_scc_handler->setCurrentContext(this);
	}

	wlr::IChannelOutbound* ico = dynamic_cast<wlr::IChannelOutbound*>(this->m_handler);	
	if (ico) ico->channelWrite(this, buf);
    if (this->m_prev) this->m_prev->invokeChannelWrite(buf);	
}

W_DEFINITION1(invokeHandler, handler, wlr::ByteBuf*)
W_DEFINITION1(invokeExceptioned, exceptioned, wlr::Exception*)

wlr::DefaultChannelHandlerContext::~DefaultChannelHandlerContext()
{
	if (this->m_handler) { 
		delete this->m_handler;
		this->m_handler = NULL;
	}
	// LOG_DEBUG("wlr::DefaultChannelHandlerContext::~DefaultChannelHandlerContext = %d\n", this);
	//if (this->m_prev) this->m_prev->m_next = this->m_next;
	//if (this->m_next) this->m_next->m_prev = this->m_prev;
}


#undef W_ASSIGNMENT_CHC
#undef W_DEFINITION0
#undef W_DEFINITION1


wlr::HeadChannelHandlerContext::HeadChannelHandlerContext(wlr::SocketChannel* socket_channel, wlr::IHandler* handler)
    : DefaultChannelHandlerContext(socket_channel, handler, "head")
{}

void wlr::HeadChannelHandlerContext::invokeChannelRead(wlr::ByteBuf* buf)
{
	wlr::ByteBuf* readBuf = buf;
	if (!readBuf) readBuf = new wlr::ByteBuf(1024 << 10);
	int len;
	wlr::SocketChannel* socket_channel = this->socketChannel();
	LOG_DEBUG("socket channel id = %d, start read\n", socket_channel->channelId().id());
	try {
		len = socket_channel->read(readBuf);
		if (len > 0) {
		//while((len = socket_channel->read(readBuf)) != -1)
		//{
			LOG_DEBUG("socket channel id = %d, read buf len = %d\n", socket_channel->channelId().id(), len);
			this->m_next->invokeChannelRead(readBuf);
			readBuf->clear();
		}
	} catch(wlr::Exception e) {
		W_DELETE(readBuf);
		LOG_ERROR("channel read error, error message = %s\n", e.message().c_str());
		throw e;
    } catch(...) {
		LOG_ERROR("channel read error, channel id = %d\n", socket_channel->channelId().id());
    }

	W_DELETE(readBuf);
}

void wlr::HeadChannelHandlerContext::invokeChannelWrite(wlr::ByteBuf* buf)
{
	if (this->m_scc_handler) 
	{
		this->m_scc_handler->setInOutbound(false);
		this->m_scc_handler->setCurrentContext(NULL);
	}

	if (buf)
	{
		int len = this->m_scc_handler->writeFlush(*buf);
		LOG_DEBUG("socket channel id = %d, write buf len = %d\n", this->m_scc_handler->channelId().id(), len);
	}	
}



wlr::TailChannelHandlerContext::TailChannelHandlerContext(wlr::SocketChannel* socket_channel, wlr::IHandler* handler)
	: DefaultChannelHandlerContext(socket_channel, handler, "tail")
{}

void wlr::TailChannelHandlerContext::invokeChannelRead(wlr::ByteBuf* buf)
{
	LOG_DEBUG("tail invoke read, buf = %d, buf readable = %d, writeable = %d\n", buf, buf->readable(), buf->writeable());
	buf->clear();
	this->invokeChannelWrite(buf);
}

void wlr::TailChannelHandlerContext::invokeChannelWrite(wlr::ByteBuf* buf)
{
	LOG_DEBUG("this->m_scc_handler = %d, this->m_prev = %d, this = %d\n", this->m_scc_handler, this->m_prev, this);
	if (this->m_scc_handler) this->m_scc_handler->setInOutbound(true);
	if (this->m_prev) this->m_prev->invokeChannelWrite(buf);
}

