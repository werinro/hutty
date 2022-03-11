#ifndef __WLR__HUTTY__CHANNEL__H
#define __WLR__HUTTY__CHANNEL__H


#include "buf.h"
#include "socket.h"
#include "ichannelpipeline.h"
#include "../utils/constant.h"


namespace wlr
{


class ChannelId;
class Channel
{
public:
	Channel();
    wlr::ChannelId& channelId();	
	virtual int read(wlr::ByteBuf *buf, int len = -1) throw (wlr::SocketException) = 0;
    virtual int write(wlr::ByteBuf &buf, int len = -1) throw (wlr::SocketException) = 0;
	virtual ~Channel();

private:
	wlr::ChannelId* m_channel_id;
};


class ChannelId
{
	friend class Channel;
	ChannelId();
public:
	bool operator==(const ChannelId& channel_id) const;
	bool operator<(const ChannelId& channel_id) const;
	bool operator>(const ChannelId& channel_id) const;
	inline size_t id() { return this->m_id; }
private:
	size_t m_id;
};


class SocketChannel : public Channel, public IChannelPipeline
{
	friend class ServerSocketChannel;
public:
	enum state
	{
		s_init,
		s_connected,
		s_closed,
	};

	explicit SocketChannel(wlr::Socket *socket);
	virtual ~SocketChannel() override;

	virtual int read(wlr::ByteBuf *buf, int len = -1) throw (wlr::SocketException) override;
	virtual int write(wlr::ByteBuf &buf, int len = -1) throw (wlr::SocketException) override;
	virtual bool tryRead();
	virtual bool tryWrite();
	virtual bool isClose();
	virtual void connect(std::string host, int port);
	virtual void flush();
	virtual void close();
protected:
	wlr::Socket* m_socket;
	wlr::SocketChannel::state m_f = wlr::SocketChannel::s_init;
};


class SocketChannelFactory
{
public:
	virtual wlr::SocketChannel* createSocketChannel(wlr::Socket *socket) = 0;
	virtual ~SocketChannelFactory() {}
};


class ServerSocketChannel : public Channel
{
public:
	explicit ServerSocketChannel(wlr::ServerSocket* ssocket, int backlog = 128);
	virtual void setSocketChannelFactory(wlr::SocketChannelFactory*);
	virtual wlr::SocketChannel* accept();
	virtual void close();
	virtual int read(wlr::ByteBuf *buf, int len = -1) throw (wlr::SocketException) override;
    virtual int write(wlr::ByteBuf &buf, int len = -1) throw (wlr::SocketException) override;
	virtual ~ServerSocketChannel() override;
private:
	wlr::ServerSocket* m_s_socket;
protected:
	wlr::SocketChannelFactory* m_sc_factory = NULL;
};



}



#endif
