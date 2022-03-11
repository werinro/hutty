#include <atomic>
#include "channel.h"


wlr::Channel::Channel()
{
	this->m_channel_id = new wlr::ChannelId();
}

wlr::Channel::~Channel()
{
	LOG_DEBUG("wlr::Channel::~Channel(%d)\n", this->m_channel_id);
    delete this->m_channel_id;
}

wlr::ChannelId& wlr::Channel::channelId()
{ return *this->m_channel_id; }



wlr::ChannelId::ChannelId()
{
	static std::atomic<size_t> s_id(999999);
    s_id++;
    this->m_id = s_id.load();	
}

bool wlr::ChannelId::operator==(const ChannelId& channel_id) const
{
	return this->m_id == channel_id.m_id;
}

bool wlr::ChannelId::operator<(const ChannelId& channel_id) const
{
    return this->m_id < channel_id.m_id;
}

bool wlr::ChannelId::operator>(const ChannelId& channel_id) const
{
    return this->m_id > channel_id.m_id;
}



wlr::SocketChannel::SocketChannel(wlr::Socket *socket)
	: m_socket(socket)
{}

wlr::SocketChannel::~SocketChannel()
{
	if (this->m_f != wlr::SocketChannel::s_closed) {
		this->close();
	}
	W_DELETE(this->m_socket);
}

int wlr::SocketChannel::read(wlr::ByteBuf *buf, int len) throw (wlr::SocketException)
{ 
	int length = buf->writeable();
	if (len > length) len = length;

	char buffer[length];
	len = this->m_socket->read(buffer, len);
	if (!len) {
		this->m_f = wlr::SocketChannel::s_closed;
		W_THROW(SocketException, "channel is close");
	}

	len = buf->write(buffer, 0, len);
	return len;
}

int wlr::SocketChannel::write(wlr::ByteBuf &buf, int len) throw (wlr::SocketException)
{
	char buffer[len];
	len = buf.readBytes(buffer, len);
	int size = this->m_socket->write(buffer, len);
	if (size == -1) {
        this->m_f = wlr::SocketChannel::s_closed;
        W_THROW(SocketException, "channel is close");
    }

	return size;
}

bool wlr::SocketChannel::tryRead() { return false; }
bool wlr::SocketChannel::tryWrite() { return false; }
bool wlr::SocketChannel::isClose() 
{ return this->m_f == wlr::SocketChannel::s_closed; }

void wlr::SocketChannel::connect(std::string host, int port)
{
	try
	{
		this->m_socket->connect(host, port);
		this->m_f = wlr::SocketChannel::s_connected;
	} 
	catch(wlr::SocketException se)
	{
		LOG_ERROR("wlr::SocketChannel::connect error, %s\n", se.message().c_str());
	}
}

void wlr::SocketChannel::flush()
{
}

void wlr::SocketChannel::close()
{
	this->flush();
	this->m_socket->close();
	this->m_f = wlr::SocketChannel::s_closed;
}



wlr::ServerSocketChannel::ServerSocketChannel(wlr::ServerSocket* ssocket, int backlog)
	: m_s_socket(ssocket)
{
	if (!ssocket) W_THROW(NullPointerException, "ssocket");
	this->m_s_socket->bind();
	this->m_s_socket->listen(backlog);
}

void wlr::ServerSocketChannel::setSocketChannelFactory(wlr::SocketChannelFactory* factory)
{ this->m_sc_factory = factory; }

wlr::SocketChannel* wlr::ServerSocketChannel::accept()
{
    wlr::Socket* socket = this->m_s_socket->accept();
	wlr::SocketChannel* sc;
	if (this->m_sc_factory)
		sc = this->m_sc_factory->createSocketChannel(socket);
	else
		sc = new wlr::SocketChannel(socket);
	sc->m_f = wlr::SocketChannel::s_connected;
	return sc;
}

void wlr::ServerSocketChannel::close()
{
	this->m_s_socket->close();
}

int wlr::ServerSocketChannel::read(wlr::ByteBuf *buf, int len) throw (wlr::SocketException) {}
int wlr::ServerSocketChannel::write(wlr::ByteBuf &buf, int len) throw (wlr::SocketException) {}

wlr::ServerSocketChannel::~ServerSocketChannel()
{
	this->close();
	W_DELETE(this->m_s_socket);
}

