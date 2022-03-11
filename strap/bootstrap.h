#ifndef __WLR__BOOTSTRAP__H
#define __WLR__BOOTSTRAP__H



#include <typeinfo>
#include <stdlib.h>

#include "../structure/maps.h"
#include "../utils/logger.h"
#include "../io/ichannelpipeline.h"
#include "../handle/initializerchannelhandler.h"
#include "channelfuture.h"

namespace wlr
{



class Option
{
public:
	void operator=(const wlr::Option& option);
	Option(const wlr::Option& option);
	explicit Option(const char* name);
	virtual ~Option();
	const char* name();
protected:
	const char* m_name;
};


template<class Value>
class ChannelOption : public Option
{
	friend class AbstractBootstrap;
public:
	ChannelOption();
	ChannelOption(Value value);
	ChannelOption(wlr::Option &option);
	Value get() throw(wlr::ClassCastException);

private:
	wlr::ChannelOption<Value>& set(Value value);
	
	template<class V>
	inline bool isType(V v) {return this->name() == typeid(v).name();}
	
private:
	Value m_value;
};


class AbstractBootstrap
{
public:
	AbstractBootstrap();
	virtual ~AbstractBootstrap();
	virtual AbstractBootstrap* group(wlr::EventLoopGroup* boos, wlr::EventLoopGroup* worker = NULL) = 0;
	virtual AbstractBootstrap* channel(wlr::SocketChannelFactory* factory) = 0;

	template<class Value>
	AbstractBootstrap* option(wlr::ChannelOption<Value>* option, Value value);

	virtual AbstractBootstrap* handler(wlr::InitializerChannelHandler* handler) = 0;
	virtual AbstractBootstrap* listener(wlr::FutureListener* listener) = 0;
	virtual AbstractBootstrap* address(int port, std::string host) = 0;
	virtual wlr::ChannelFuture* sync() throw(wlr::Exception) = 0;
	virtual void waitForClose() = 0;

	static wlr::ChannelOption<int> Backlog;
	static wlr::ChannelOption<size_t> BufferSize;

	class Selector
	{
	public:
		typedef enum Event { 
			ACCEPT, READ, WRITE, 
		} Event;

		explicit Selector(wlr::SocketChannel* channel, wlr::AbstractBootstrap::Selector::Event event)
			: m_channel(channel)
			, m_event(event)
		{}
		
		inline wlr::AbstractBootstrap::Selector::Event event()
		{ return this->m_event; }

		inline wlr::SocketChannel* channel()
		{ return this->m_channel; }
	private:
		wlr::SocketChannel *m_channel;
		wlr::AbstractBootstrap::Selector::Event m_event;
	};

protected:
	std::map<wlr::AbstractBootstrap::Selector::Event, std::list<wlr::AbstractBootstrap::Selector*> > m_selector_map;
	std::map<wlr::ChannelId, wlr::AbstractBootstrap::Selector*> m_read_map;
	std::map<wlr::ChannelId, wlr::AbstractBootstrap::Selector*> m_write_map;
};


class ServerBootstrap : public AbstractBootstrap
{
public:
	ServerBootstrap(wlr::EventLoopGroup* boos = NULL, wlr::EventLoopGroup* worker = NULL);
	virtual ~ServerBootstrap() override;
    virtual wlr::AbstractBootstrap* group(wlr::EventLoopGroup* boos, wlr::EventLoopGroup* worker) override;
    virtual wlr::AbstractBootstrap* channel(wlr::SocketChannelFactory* factory) override;
	virtual wlr::AbstractBootstrap* handler(wlr::InitializerChannelHandler* handler) override;
    virtual wlr::AbstractBootstrap* listener(wlr::FutureListener* listener) override;
	virtual wlr::AbstractBootstrap* address(int port, std::string host) override;
    virtual wlr::ChannelFuture* sync() throw(wlr::Exception) override;
    virtual void waitForClose() override;

protected:
	wlr::EventLoopGroup* m_boos;
	wlr::EventLoopGroup* m_worker;
	wlr::SocketChannelFactory* m_socket_factory = NULL;
	wlr::ServerSocket* m_ssocket = NULL;
	wlr::InitializerChannelHandler* m_pipeline_handler = NULL;
	wlr::ChannelFuture* m_future;

private:
	static void* boos(void*);
	static void* workerSelect(void*);
	static void* workerHandler(void*);
};



}

#endif
