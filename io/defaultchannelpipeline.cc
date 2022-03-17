#include "defaultchannelpipeline.h"


wlr::DefaultChannelPipeline::DefaultChannelPipeline(wlr::SocketChannel* socket_channel)
	: ChannelPipeline(socket_channel)
{
	this->m_scc_handler = new wlr::SocketChannelContextHandler(socket_channel);
	this->m_head = new wlr::HeadChannelHandlerContext(this->m_scc_handler);
	this->m_tail = new wlr::TailChannelHandlerContext(this->m_scc_handler);
	this->m_head->insertNext(this->m_tail);
}

wlr::DefaultChannelPipeline::~DefaultChannelPipeline()
{
	wlr::DefaultChannelHandlerContext* next;
	wlr::DefaultChannelHandlerContext* dchc = this->m_head;
	while(dchc)
	{
		next = dchc->removeNext();
		W_DELETE(dchc);
		dchc = next;
	}

	W_DELETE(this->m_scc_handler);
}

wlr::ChannelPipeline* wlr::DefaultChannelPipeline::addFirst(wlr::ChannelHandler* handler, std::string name)
{
	this->m_head->insertNext(new wlr::DefaultChannelHandlerContext(this->m_scc_handler, handler, name));
}

wlr::ChannelPipeline* wlr::DefaultChannelPipeline::addLast(wlr::ChannelHandler* handler, std::string name)
{
	this->m_tail->insertPrev(new wlr::DefaultChannelHandlerContext(this->m_scc_handler, handler, name));
}

wlr::ChannelHandlerContext* wlr::DefaultChannelPipeline::handlerContext(std::string name)
{
	wlr::DefaultChannelHandlerContext* dchc = this->m_head;
    while(dchc)
    {
		if (name.compare(dchc->name()))
			return dchc;
	}
	return NULL;
}

wlr::ChannelHandlerContext* wlr::DefaultChannelPipeline::headHandlerContext()
{ return this->m_head; }

wlr::ChannelHandlerContext* wlr::DefaultChannelPipeline::tailHandlerContext()
{ return this->m_tail; }
	
#define W_DEBUG_PRINT(func) LOG_DEBUG("socket channel %s, id = %d\n", #func, this->m_sc->channelId().id());

void wlr::DefaultChannelPipeline::fireConnected()
{ 
	W_DEBUG_PRINT(fireConnected)
	this->m_head->invokeConnected(); 
}

void wlr::DefaultChannelPipeline::fireRegistered()
{
	W_DEBUG_PRINT(fireRegistered) 
	this->m_head->invokeRegistered(); 
}

void wlr::DefaultChannelPipeline::fireChannelRead()
{ 
	W_DEBUG_PRINT(fireChannelRead)
	if (this->m_destroyed) return;
	this->m_head->invokeChannelRead(NULL); 
}

void wlr::DefaultChannelPipeline::fireClosed()
{ 
	W_DEBUG_PRINT(fireClosed)
	this->m_head->invokeClosed(); 
}

void wlr::DefaultChannelPipeline::fireDestroyed()
{ 
	W_DEBUG_PRINT(fireDestroyed)
	this->m_destroyed = 1;
	this->m_head->invokeDestroyed(); 
}

void wlr::DefaultChannelPipeline::fireChannelWrite()
{ 
	W_DEBUG_PRINT(fireChannelWrite)
	if (this->m_destroyed) return;
	this->m_tail->invokeChannelWrite(NULL); 
}

void wlr::DefaultChannelPipeline::fireHandler()
{ 
	W_DEBUG_PRINT(fireHandler)
	this->m_head->invokeHandler(NULL); 
}

void wlr::DefaultChannelPipeline::fireExceptioned(wlr::Exception* ex)
{ 
	W_DEBUG_PRINT(fireExceptioned)
	if (this->m_destroyed) return;
	this->m_head->invokeExceptioned(ex); 
}

void wlr::DefaultChannelPipeline::fireEventLoopGroup(wlr::EventLoopGroup* elp)
{
	wlr::DefaultChannelHandlerContext* dchc = this->m_head->next();
    while(dchc)
    {
		dchc->setEventLoopGroup(elp);
		dchc = dchc->next();
	}
	// std::dynamic_cast<>
}

bool wlr::DefaultChannelPipeline::destroyed() 
{ return this->m_destroyed; }
