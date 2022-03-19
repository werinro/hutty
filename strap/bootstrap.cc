#include "bootstrap.h"
#include "../schedule/thread.h"




void wlr::Option::operator=(const wlr::Option& option) 
{
	this->m_name = option.m_name;
}
	
wlr::Option::Option(const wlr::Option& option)
{
	this->m_name = option.m_name;
}

wlr::Option::Option(const char* name) 
	: m_name(name) 
{
	// printf("this->m_name = %s\n", name);
}

wlr::Option::~Option() {}

const char* wlr::Option::name()
{ return this->m_name; }


template<class Value>
wlr::ChannelOption<Value>::ChannelOption() 
	: Option(typeid(Value).name()) 
{}

template<class Value>
wlr::ChannelOption<Value>::ChannelOption(Value value) 
	: Option(typeid(Value).name())
	, m_value(value) 
{}

template<class Value>
wlr::ChannelOption<Value>::ChannelOption(wlr::Option &option)
{ this->m_name = option.name(); }

template<class Value>
Value wlr::ChannelOption<Value>::get() throw(wlr::ClassCastException) 
{
	if (!isType(this->m_value)) 
		W_THROW(ClassCastException, this->name());
	return this->m_value;
}

template<class Value>
wlr::ChannelOption<Value>& wlr::ChannelOption<Value>::set(Value value) 
{
	this->m_value = value; 
	return *this;
}
	


wlr::AbstractBootstrap::AbstractBootstrap() {}
wlr::AbstractBootstrap::~AbstractBootstrap() {}

template<class Value>
wlr::AbstractBootstrap* wlr::AbstractBootstrap::option(wlr::ChannelOption<Value> *option, Value value)
{
    option->set(value);
    return this;
}

wlr::ChannelOption<int> wlr::AbstractBootstrap::Backlog = wlr::ChannelOption<int>(128);
wlr::ChannelOption<size_t> wlr::AbstractBootstrap::BufferSize = wlr::ChannelOption<size_t>(1 << 15);



wlr::ServerBootstrap::ServerBootstrap(wlr::EventLoopGroup* boos, wlr::EventLoopGroup* worker) 
	: m_boos(boos)
	, m_worker(worker)
{
	this->m_future = new wlr::ChannelFuture();
}

wlr::ServerBootstrap::~ServerBootstrap()
{
	delete this->m_future;
	if (this->m_socket_factory) delete this->m_socket_factory;
	if (this->m_pipeline_handler) delete this->m_pipeline_handler;
	if (this->m_boos) delete this->m_boos;
	if (this->m_worker) delete this->m_worker;
}

wlr::AbstractBootstrap* wlr::ServerBootstrap::group(wlr::EventLoopGroup* boos, wlr::EventLoopGroup* worker)
{
	this->m_boos = boos;
	this->m_worker = worker;
	return this;
}

wlr::AbstractBootstrap* wlr::ServerBootstrap::channel(wlr::SocketChannelFactory* factory)
{
	this->m_socket_factory = factory;
	return this;
}

wlr::AbstractBootstrap* wlr::ServerBootstrap::handler(wlr::InitializerChannelHandler *handler)
{
	this->m_pipeline_handler = handler;
	return this;	
}

wlr::AbstractBootstrap* wlr::ServerBootstrap::listener(wlr::FutureListener *listener)
{
	this->m_future->addListener(listener);
	return this;
}

wlr::AbstractBootstrap* wlr::ServerBootstrap::address(int port, std::string host)
{
	this->m_ssocket = new wlr::ServerSocket(port, host);
	return this;
}

wlr::ChannelFuture* wlr::ServerBootstrap::sync() throw(wlr::Exception)
{
	if (!this->m_boos || !this->m_worker)
		W_THROW(NullPointerException, "boos event pool or worker event pool is null");

	this->m_future->init("start init");
	wlr::Future* f = this->m_boos->submit(new wlr::FunctionRunnable([](void* wsb) -> void* {
		wlr::ServerBootstrap* s_bootstrap = (wlr::ServerBootstrap*)wsb;
		if (!s_bootstrap->m_socket_factory) {
			s_bootstrap->m_future->error("socket factory is null");	
			return NULL;
		}

		if (!s_bootstrap->m_pipeline_handler) {
            s_bootstrap->m_future->error("initializer channel handler is null");
            return NULL;
        }

		if (!s_bootstrap->m_ssocket) {
            s_bootstrap->m_future->error("not set address");
            return NULL;
        }
		
		// 初始化队列
		s_bootstrap->m_selector_map.insert(wlr::toPair(wlr::AbstractBootstrap::Selector::ACCEPT, std::list<wlr::AbstractBootstrap::Selector*>()));
		//s_bootstrap->m_selector_map.insert(wlr::toPair(wlr::AbstractBootstrap::Selector::READ, std::list<wlr::AbstractBootstrap::Selector*>()));
		//s_bootstrap->m_selector_map.insert(wlr::toPair(wlr::AbstractBootstrap::Selector::WRITE, std::list<wlr::AbstractBootstrap::Selector*>()));
		
		wlr::Future* f0 = s_bootstrap->m_boos->submit(&wlr::ServerBootstrap::boos, s_bootstrap);
		wlr::Future* f1 = s_bootstrap->m_worker->submit(&wlr::ServerBootstrap::workerSelect, s_bootstrap);
		if (f0) delete f0;
		if (f1) delete f1;
		s_bootstrap->m_future->done("start done");
		return s_bootstrap;
	}, this));

	if (!f || !f->get(100000)) {
		this->m_boos->shutdown();
		this->m_worker->shutdown();
	}	
	return this->m_future;
}

void wlr::ServerBootstrap::waitForClose()
{
	while(1)
	{
		sleep(10);
		if (this->m_boos->isShutdown() && this->m_worker->isShutdown()) {
			break;
		}
	}
}

void* wlr::ServerBootstrap::boos(void* wsb)
{
	wlr::ServerBootstrap* s_bootstrap = (wlr::ServerBootstrap*)wsb;
    auto iter_pair = s_bootstrap->m_selector_map.find(wlr::AbstractBootstrap::Selector::ACCEPT);
	wlr::ServerSocketChannel* ss_channel = NULL;
	wlr::Thread* self = wlr::Thread::getCurrentThread();

	do {
		try {
			// todo 通过工厂获取通道对象
			int backlog = wlr::AbstractBootstrap::Backlog.get();
			ss_channel = new wlr::ServerSocketChannel(s_bootstrap->m_ssocket, backlog);
			LOG_INFO("server socket start up, listner ip %s port %d\n", s_bootstrap->m_ssocket->host().c_str(), s_bootstrap->m_ssocket->port());
			ss_channel->setSocketChannelFactory(s_bootstrap->m_socket_factory);
			break;
		} catch(wlr::Exception e) {
			LOG_ERROR("%s\n%s", e.message().c_str(), e.stackString().c_str());
		} catch(...) {
			wlr::Thread::printStack();
		}

		s_bootstrap->m_boos->shutdown();
		s_bootstrap->m_worker->shutdown();
		return NULL;
	} while(0);


	while(1)
	{
		try {
			wlr::SocketChannel* socket_channel = ss_channel->accept();
			// todo 初始化通道
			s_bootstrap->m_pipeline_handler->init(socket_channel, s_bootstrap->m_worker);
			// 获取通道管道调用连接处理器
			wlr::ChannelPipeline* channel_pipeline = socket_channel->channelPipeline();
			channel_pipeline->fireConnected();
			// 将通道注册到选择队列
			wlr::AbstractBootstrap::Selector* accept_selector = new wlr::AbstractBootstrap::Selector(socket_channel, wlr::AbstractBootstrap::Selector::ACCEPT);
			s_bootstrap->m_accept_mutex.lock();
			(*iter_pair).second.push_back(accept_selector);
			s_bootstrap->m_accept_mutex.unlock();
			// 调用通道注册处理器
			channel_pipeline->fireRegistered();
			
			// todo 处理任务队列任务
			//
			if (self->isInterrupt()) break;
		} catch(wlr::Exception e) {
            LOG_ERROR("%s\n%s", e.message().c_str(), e.stackString().c_str());
        } catch(...) {
			wlr::Thread::printStack();
        }
	}
	return 0;
}

void* wlr::ServerBootstrap::workerSelect(void* wsb)
{
	// 维护注册列表已连接通道状态
	wlr::ServerBootstrap* s_bootstrap = (wlr::ServerBootstrap*)wsb;
	auto iter_pair = s_bootstrap->m_selector_map.find(wlr::AbstractBootstrap::Selector::ACCEPT);
	wlr::Thread* self = wlr::Thread::getCurrentThread();

	while(1)
	{
		size_t size = (*iter_pair).second.size();
		wlr::Thread::sleep(300);
		for (auto l_iter = (*iter_pair).second.begin(); l_iter != (*iter_pair).second.end(); l_iter++)
		{
			wlr::SocketChannel* socket_channel = (*l_iter)->channel();
			try {
				// 判断通道是否关闭
				if (socket_channel->isClose())
				{	
					if (!socket_channel->channelPipeline()->destroyed()) {
						socket_channel->channelPipeline()->fireDestroyed();
					} else {
						socket_channel->channelPipeline()->fireDestroyed();
						delete socket_channel;
						delete (*l_iter);
						s_bootstrap->m_accept_mutex.lock();
						(*iter_pair).second.erase(l_iter);
						s_bootstrap->m_accept_mutex.unlock();
						l_iter = (*iter_pair).second.begin();
						l_iter--;
						continue;
					}		
				}
			
				// 判断通道是否有写入写出事件
				wlr::ChannelId channel_id = socket_channel->channelId();
#define W_TRY_(iter, map, try_func, event) \
	auto iter = map.find(channel_id); \
	if (iter == map.end() && socket_channel->try_func()) \
	{ \
		LOG_INFO("select %s, channel id = %d\n", #try_func, channel_id); \
		wlr::Future* f = NULL; \
		s_bootstrap->m_rw_map_mutex.lock(); \
		map.insert(wlr::toPair(channel_id, (*l_iter))); \
		s_bootstrap->m_rw_map_mutex.unlock(); \
		wlr::AbstractBootstrap::Selector* select_event = new wlr::AbstractBootstrap::Selector(socket_channel, event); \
        f = s_bootstrap->m_worker->submit(&wlr::ServerBootstrap::workerHandler, new std::pair<wlr::AbstractBootstrap::Selector*, wlr::ServerBootstrap*>(select_event, s_bootstrap)); \
		if (f) delete f; \
	}
				W_TRY_(read_iter, s_bootstrap->m_read_map, tryRead, wlr::AbstractBootstrap::Selector::READ)
				W_TRY_(write_iter, s_bootstrap->m_write_map, tryWrite, wlr::AbstractBootstrap::Selector::WRITE)

#undef W_TRY_

			} catch(wlr::Exception e) {
				LOG_ERROR("%s\n%s", e.message().c_str(), e.stackString().c_str());
				socket_channel->channelPipeline()->fireExceptioned(&e);
			} catch(...) {
				wlr::Thread::printStack();
			}
		}
		if (self->isInterrupt()) break;
	}
	
	return 0;
}

void* wlr::ServerBootstrap::workerHandler(void* event_pair)
{
	std::pair<wlr::AbstractBootstrap::Selector*, wlr::ServerBootstrap*> *e_pair = (std::pair<wlr::AbstractBootstrap::Selector*, wlr::ServerBootstrap*>*)event_pair;
	wlr::AbstractBootstrap::Selector* selector = e_pair->first;
	wlr::ServerBootstrap* s_bootstrap = e_pair->second;
	wlr::SocketChannel* socket_channel = selector->channel();

	try {
		// 处理事件
		if (selector->event() == wlr::AbstractBootstrap::Selector::READ)
		{
			s_bootstrap->m_rw_map_mutex.lock();
			s_bootstrap->m_read_map.erase(socket_channel->channelId());
			s_bootstrap->m_rw_map_mutex.unlock();
			socket_channel->channelPipeline()->fireChannelRead();
		}
		else if (selector->event() == wlr::AbstractBootstrap::Selector::WRITE)
		{
			// socket_channel->channelPipeline()->fireChannelWrite();
			s_bootstrap->m_rw_map_mutex.lock();
			s_bootstrap->m_write_map.erase(socket_channel->channelId());
			s_bootstrap->m_rw_map_mutex.unlock();
			socket_channel->flush();
		}
	} catch(wlr::Exception e) {
		LOG_ERROR("%s\n%s", e.message().c_str(), e.stackString().c_str());
		socket_channel->channelPipeline()->fireExceptioned(&e);
	} catch(...) {
		wlr::Thread::printStack();
	}
	
	delete selector;
	delete e_pair;
    return 0;
}

