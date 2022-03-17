#ifndef __WLR__THREADEVENTLOOPGROUP__H
#define __WLR__THREADEVENTLOOPGROUP__H


#include "eventloopgroup.h"
#include "thread.h"


namespace wlr
{


class ThreadEventLoopGroup : public EventLoopGroup
{
public:
	explicit ThreadEventLoopGroup(int max_threads = -1, int max_queues = -1);
	virtual wlr::Future* submit(wlr::Runnable* runnable);
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
