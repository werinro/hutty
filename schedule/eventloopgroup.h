#ifndef __WLR__EVENTLOOPGROUP__H
#define __WLR__EVENTLOOPGROUP__H


#include "schedule.h"
#include "schedulefuture.h"


namespace wlr
{


class Runable;
class EventLoopGroup
{
public:
	virtual	~EventLoopGroup() {}
	virtual wlr::Future* submit(wlr::Runable* runable) = 0;
	virtual wlr::Future* submit(void* (*func)(void*), void* args = 0) = 0;
	virtual wlr::ScheduleFuture* schedule(wlr::Schedule* schedule) = 0;
	virtual void shutdown() = 0;
	virtual bool isShutdown() = 0;
};


}


#endif
