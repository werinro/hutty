#include "threadeventloopgroup.h"
#include "../utils/environment.h"


wlr::ThreadEventLoopGroup::ThreadEventLoopGroup(int max_threads, int max_queues)
{
	if (max_threads < 1) {
		int cores = wlr::Environment::get(ENV_CORES);
		max_threads = cores << 1;
	}

	if (max_queues < 1) {
		size_t memory_free = wlr::Environment::get(ENV_MEMORY_FREE);
		max_queues = static_cast<int>(memory_free >> 10);
	}

	this->m_thread_pool = new wlr::ThreadPool(max_threads >> 1, max_threads, max_queues);
}

wlr::ThreadEventLoopGroup::~ThreadEventLoopGroup()
{
	if (this->m_thread_pool) delete this->m_thread_pool;
}

wlr::Future* wlr::ThreadEventLoopGroup::submit(wlr::Runnable* runnable)
{
	return this->m_thread_pool->submit(runnable);
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

