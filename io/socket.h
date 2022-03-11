#ifndef __WLR__SOCKET__H
#define __WLR__SOCKET__H

#include <errno.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../utils/logger.h"
#include "../utils/exception.h"



namespace wlr
{


class AbstractSocket
{
public:
	virtual ~AbstractSocket() 
	{ LOG_DEBUG("~AbstractSocket(%d)\n", this); if (!this->isClose()) this->close(); }

	virtual void close() = 0;
	virtual bool isClose() const = 0;
	virtual int port() const = 0;
	virtual std::string host() const = 0;
};


class ServerSocket;
class Socket
{
   friend class ServerSocket;
public:
    Socket();
    virtual ~Socket();

    void connect(std::string host, int port);
    int read(char *buf, int len);
    int recv(char *buf, int len, int flag = 0);
    int send(char *buf, int len, int flag = 0);
    int write(char *buf, int len);
    int write(std::string message);

    virtual int port() const;
    virtual std::string host() const;
    virtual void close();
    virtual bool isClose() const;
private:

    int m_fg;
    int m_client;

    int m_port;
    std::string m_host;
};



class ServerSocket
{
public:
    explicit ServerSocket(int port, std::string host = "0.0.0.0");
    virtual ~ServerSocket();
    
    int bind();
    int listen(int backlog = 128);
    wlr::Socket* accept();

    virtual int port() const;
    virtual std::string host() const;
    virtual void close();
    virtual bool isClose() const;

private:

    int m_fg;
    int m_server;
	int m_port;
    std::string m_host;
    struct sockaddr_in m_addr;
};




}


#endif
