#include "threadeventloopgroup.h"


wlr::ThreadEventLoopGroup::ThreadEventLoopGroup(wlr::ThreadPool* thread_pool)
	: m_thread_pool(thread_pool)
{}

wlr::ThreadEventLoopGroup::~ThreadEventLoopGroup()
{
	if (this->m_thread_pool) delete this->m_thread_pool;
}

wlr::Future* wlr::ThreadEventLoopGroup::submit(wlr::Runable* runable)
{
	return this->m_thread_pool->submit(runable);
}

wlr::Future* wlr::ThreadEventLoopGroup::submit(void* (*func)(void*), void* args)
{
	return this->m_thread_pool->submit(func, args);
}

wlr::ScheduleFuture* wlr::ThreadEventLoopGroup::schedule(wlr::Schedule* schedule)
{
	return NULL;
}

void wlr::ThreadEventLoopGroup::shutdown()
{
	this->m_thread_pool->shutdown();
}

bool wlr::ThreadEventLoopGroup::isShutdown()
{
	return this->m_thread_pool->isShutdown();
}

