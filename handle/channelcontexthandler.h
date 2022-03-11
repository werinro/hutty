#ifndef __WLR__CHANNELCONTEXTHANDLER__H
#define __WLR__CHANNELCONTEXTHANDLER__H


#include "handler.h"
#include "../schedule/threadlocal.h"



namespace wlr
{


class SocketChannelContextHandler : public SocketChannel
{
public:
	SocketChannelContextHandler(wlr::SocketChannel* socketChannel);
    virtual int read(wlr::ByteBuf *buf, int len = -1) throw (wlr::SocketException) override;
    virtual int write(wlr::ByteBuf &buf, int len = -1) throw (wlr::SocketException) override;
	virtual int writeFlush(wlr::ByteBuf &buf, int len = -1) throw (wlr::SocketException);
    virtual bool tryRead() override;
    virtual bool tryWrite() override;
    virtual bool isClose() override;
    virtual void connect(std::string host, int port) override;
    virtual void flush() override;
    virtual void close() override;

	bool inOutbound();
	void setInOutbound(bool);
	wlr::ChannelHandlerContext* currentContext();
	void setCurrentContext(wlr::ChannelHandlerContext*);
private:
	wlr::ThreadLocal<wlr::ChannelHandlerContext*> m_current_context;
	wlr::ThreadLocal<bool> m_in_outbound;
	wlr::SocketChannel* m_sc;
};



}

#endif
