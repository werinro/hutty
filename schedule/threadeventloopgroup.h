#ifndef __WLR__THREADEVENTLOOPGROUP__H
#define __WLR__THREADEVENTLOOPGROUP__H


#include "eventloopgroup.h"
#include "thread.h"


namespace wlr
{


class ThreadEventLoopGroup : public EventLoopGroup
{
public:
	explicit ThreadEventLoopGroup(wlr::ThreadPool* thread_pool);
	virtual wlr::Future* submit(wlr::Runable* runable);
	virtual wlr::Future* submit(void* (*func)(void*), void* args);
	virtual wlr::ScheduleFuture* schedule(wlr::Schedule* schedule);
	virtual void shutdown();
	virtual bool isShutdown();
	virtual ~ThreadEventLoopGroup();

protected:
	wlr::ThreadPool* m_thread_pool;
};


}


#endif
