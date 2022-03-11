#include "niosocketchannel.h"



wlr::NioSocketChannel::NioSocketChannel(wlr::Socket *socket, size_t buf_size)
	: SocketChannel(socket)
{
	this->m_pipeline = new wlr::DefaultChannelPipeline(this);
	int size = static_cast<int>(buf_size);
	if (size < 0) size = INT_MAX;
	this->m_buffer = new wlr::ByteBuf(size);
	this->m_read_buffer = new wlr::ByteBuf(size);
}

wlr::NioSocketChannel::~NioSocketChannel()
{
	if (this->m_pipeline) delete this->m_pipeline;
	if (this->m_buffer) delete this->m_buffer;
	if (this->m_read_buffer) delete this->m_read_buffer;
}

int wlr::NioSocketChannel::read(wlr::ByteBuf *buf, int len) throw (wlr::SocketException)
{
	int length = buf->writeable();
    if (len == -1 || len > length) len = length;

	int read_size = 0;
    char buffer[length];
	if (this->m_read_buffer->readable() > 0)
	{
		read_size = this->m_read_buffer->readBytes(buffer, len);
		if (read_size == len || this->m_read_buffer->readable() > 0)
		{
			buf->write(buffer, 0, read_size);
			return read_size;
		}
		buf->write(buffer, 0, read_size);
	}

    len = this->m_socket->read(buffer, len - read_size);
    if (!len) {
        this->m_f = wlr::SocketChannel::s_closed;
		this->m_pipeline->fireClosed();
        W_THROW(SocketException, "channel is close");
    }

    buf->write(buffer, 0, len);
    return read_size + len;
}

int wlr::NioSocketChannel::write(wlr::ByteBuf &buf, int len) throw (wlr::SocketException)
{
	this->m_write_mutex.lock();
	int size = this->m_buffer->writeBuf(buf, len);
	int read_size = this->m_buffer->readable();
    if (read_size >= this->m_buffer->capacity() >> 1)
	{
		wlr::SocketChannel::write(*this->m_buffer, read_size);
		this->m_buffer->clear();
	}

	this->m_write_mutex.unlock();
    return size;
}

bool wlr::NioSocketChannel::tryRead()
{
    if (this->m_read_buffer->readable() > 0) 
		return true;

    char buffer[128];
    int len = this->m_socket->recv(buffer, 1);
    if (len < 0) {
		return false;
    } else if (len == 0) {
		this->m_f = wlr::SocketChannel::s_closed;
		this->m_pipeline->fireClosed();
		return false;
	}

    this->m_read_buffer->write(buffer, 0, len);
    return true;
}

bool wlr::NioSocketChannel::tryWrite()
{
	return this->m_buffer->readable() > 0;
}

bool wlr::NioSocketChannel::isClose()
{
	return wlr::SocketChannel::isClose();
}

void wlr::NioSocketChannel::connect(std::string host, int port)
{ wlr::SocketChannel::connect(host, port); }

void wlr::NioSocketChannel::flush()
{ 
	this->m_write_mutex.lock();
	int size = this->m_buffer->readable();
	try {
		if (size)
			wlr::SocketChannel::write(*this->m_buffer, size);
	} catch (...) { }
	this->m_write_mutex.unlock();
}

void wlr::NioSocketChannel::close()
{ wlr::SocketChannel::close(); }

wlr::ChannelPipeline* wlr::NioSocketChannel::channelPipeline()
{ return this->m_pipeline; }



wlr::SocketChannel* wlr::NioSocketChannelFactory::createSocketChannel(wlr::Socket *socket)
{
	return new NioSocketChannel(socket);
}

