#include "socket.h"


wlr::Socket::Socket()
    : m_fg(0)
    , m_client(0)
    , m_port(0)
{
}

wlr::Socket::~Socket()
{
    /*if (this->m_fg != 3) {
		this->close();
    }*/
}

void wlr::Socket::connect(std::string host, int port)
{
    if (!this->m_client) {
	this->close();
	this->m_fg = 0;
    }

    this->m_client = socket(AF_INET, SOCK_STREAM, 0);
    if (this->m_client < 0) W_THROW(SocketException, "create socket error");

    struct sockaddr_in s_addr;
    //memset(&s_addr, 0, sizeof(s_addr));
    s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &s_addr.sin_addr.s_addr);

    int r = ::connect(this->m_client, (struct sockaddr *)&s_addr, sizeof(s_addr));
    if (r) {
        LOG_ERROR("socket connect fail, server port = %d, host = %s\n", port, host.c_str());
        W_THROW(SocketException, "connect error");
    }
    this->m_fg = 1;
}

int wlr::Socket::read(char *buf, int len)
{
    if (this->m_fg != 1) return -1;
    int size = ::read(this->m_client, buf, len);
    return size;
}

int wlr::Socket::recv(char *buf, int len, int flag)
{
    if (this->m_fg != 1) return -1;
    len = ::recv(this->m_client, buf, len, flag);
	if (!len) LOG_ERROR("socket is close, msg = %s\n", strerror(errno));
	if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) LOG_ERROR("socket is error, msg = %s\n", strerror(errno));
    return len;
}

int wlr::Socket::write(char *buf, int len)
{
    if (this->m_fg != 1) return -1;
    return ::write(this->m_client, buf, len);
}

int wlr::Socket::send(char *buf, int len, int flag)
{
    if (this->m_fg != 1) return -1;
	len = ::send(this->m_client, buf, len, flag);
	if (!len) LOG_ERROR("socket is close, msg = %s\n", strerror(errno));
	if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) LOG_ERROR("socket is error, msg = %s\n", strerror(errno));
    return len;
}

int wlr::Socket::write(std::string message)
{
    if (this->m_fg != 1) return -1;
    return ::write(this->m_client, message.c_str(), message.size());
}

int wlr::Socket::port() const
{ return this->m_port; }

std::string wlr::Socket::host() const
{ return this->m_host; }

bool wlr::Socket::isClose() const
{ return this->m_fg == 3; }

void wlr::Socket::close()
{
    if (this->m_fg == 3) return;
    int r = ::close(this->m_client);
    this->m_fg = 3;
    return;
}



wlr::ServerSocket::ServerSocket(int port, std::string host)
	: m_port(port)
	, m_host(host)
{
    this->m_server = socket(AF_INET, SOCK_STREAM, 0);
    if (this->m_server < 0) W_THROW(SocketException, "create server socket error");

    memset(&this->m_addr, 0, sizeof(this->m_addr));
    this->m_addr.sin_family = AF_INET;
    this->m_addr.sin_port = htons(port);
    this->m_addr.sin_addr.s_addr = inet_addr(host.c_str());
    this->m_fg = 0;
}

wlr::ServerSocket::~ServerSocket()
{
    /*if (this->m_fg != 3) {
		this->close();
    }*/
}

int wlr::ServerSocket::bind()
{
    if (this->m_fg > 0) return 0;

    int r =::bind(this->m_server, (struct sockaddr*)&this->m_addr, sizeof(this->m_addr));
    if (r) {
        char ip[64];
        LOG_ERROR("socket server bind error, port = %d, host = %s\n", ntohs(this->m_addr.sin_port), inet_ntop(AF_INET, &this->m_addr.sin_addr.s_addr, ip, sizeof(ip)));
		W_THROW(SocketException, "socket server bind error");
    }
    
    this->m_fg = 1;
}

int wlr::ServerSocket::listen(int backlog)
{
    if (this->m_fg > 1) return 0;
    if (this->m_fg == 0) this->bind();

    int r = ::listen(this->m_server, backlog);
    if (r) {
        W_THROW(SocketException, "socket server listen error");
    }

    this->m_fg = 2;
    return r;
}

wlr::Socket* wlr::ServerSocket::accept()
{
    if (this->m_fg != 2) W_THROW(SocketException, "socket server not listen");

    struct sockaddr_in c_addr;
    unsigned int addr_len = sizeof(c_addr);
    int client = ::accept(this->m_server, (struct sockaddr*)&c_addr, &addr_len);
    wlr::Socket* socket = new wlr::Socket;
    socket->m_client = client;
    char ip[64];
    socket->m_host = std::string(inet_ntop(AF_INET, &c_addr.sin_addr.s_addr, ip, sizeof(ip)));
    socket->m_port = c_addr.sin_port;
    socket->m_fg = 1;
    return socket;
}

int wlr::ServerSocket::port() const
{ return this->m_port; }

std::string wlr::ServerSocket::host() const
{ return this->m_host; }

bool wlr::ServerSocket::isClose() const
{ return this->m_fg == 3; }

void wlr::ServerSocket::close()
{
    if (this->m_fg == 3) return;

    int r = ::close(this->m_server);
    /*if (!r)
    {
        switch(r){
        case EBADF:
	    throw "无效的连接";
	    break;
	case EINTR:
	    throw "关闭信号中断";
            break;
	case EIO:
	    throw "IO错误";
            break;
        }
    }*/
    
    this->m_fg = 3;
    return;
}



