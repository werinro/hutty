#ifndef __WLR__NIOSOCKETCHANNEL__H
#define __WLR__NIOSOCKETCHANNEL__H


#include "channel.h"
#include "defaultchannelpipeline.h"


namespace wlr
{


class NioSocketChannel : public SocketChannel
{
public:
	explicit NioSocketChannel(wlr::Socket *socket, size_t buf_size = 1024 << 10);
	virtual ~NioSocketChannel() override;
    virtual int read(wlr::ByteBuf *buf, int len = -1) throw(wlr::SocketException) override;
    virtual int write(wlr::ByteBuf &buf, int len = -1) throw (wlr::SocketException) override;
	virtual bool tryRead() override;
    virtual bool tryWrite() override;
    virtual bool isClose() override;
    virtual void connect(std::string host, int port) override;
    virtual void flush() override;
    virtual void close() override;
	virtual wlr::ChannelPipeline* channelPipeline() override;

private:
	wlr::ChannelPipeline *m_pipeline;
	wlr::ByteBuf *m_buffer;
	wlr::ByteBuf *m_read_buffer;
	wlr::Mutex m_write_mutex;
};


class NioSocketChannelFactory : public SocketChannelFactory
{
public:
	virtual wlr::SocketChannel* createSocketChannel(wlr::Socket *socket) override;
}; 


}

#endif
