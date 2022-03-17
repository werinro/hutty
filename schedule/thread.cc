#include "thread.h"
#include <cxxabi.h>


wlr::Thread::Thread(void* (*func)(void*), std::string name)
    : m_exit(0)
    , m_flag(0)
    , m_func(func)
    , m_name(name)
    , m_thread(0)
{
}

wlr::Thread::~Thread()
{
    pthread_t thread = this->m_thread;
    std::string name = this->m_name.c_str();
    pid_t pid = this->m_id;
    wlr::Thread::s_mutex.lock();
    wlr::Thread::s_thread_cache.erase(pid);
    wlr::Thread::s_mutex.unlock();
	// 1 ~ 2, 已创建与执行函数中
    if ((this->m_flag - 1 ^ 2 - this->m_flag) > 0) pthread_detach(thread);
    POOL_DEBUG("thread %s-%d delete.\n", name.c_str(), pid);
}

std::string wlr::Thread::getName() const
{return this->m_name;}

pid_t wlr::Thread::getId() const
{return this->m_id;}

void* wlr::Thread::start(void* arg)
{
    wlr::Thread *thread = (wlr::Thread*)(arg);
    thread->m_id = wlr::threadId();
    pid_t pid = thread->m_id;
    std::string name = thread->m_name;

    // 获取线程pid, 将当前线程加入到缓存中
    wlr::Thread::s_mutex.lock();
    wlr::Thread::s_thread_cache.insert(std::pair<pid_t, Thread*>(thread->m_id, thread));
    wlr::Thread::s_mutex.unlock();

    POOL_DEBUG("thread %s-%d start execute.\n", name.c_str(), pid);
	void* data = NULL;
	try {
		thread->m_flag = 2;
		data = thread->m_func(thread->m_args);
		POOL_DEBUG("thread %s-%d execute done.\n", name.c_str(), pid);
	} catch (...) {
		POOL_ERROR("thread %s-%d execute error.\n", name.c_str(), pid);
		wlr::Thread::printStack();
	}

	thread = NULL;
	std::map<pid_t, Thread*>::iterator iter = wlr::Thread::s_thread_cache.find(pid);
    if (iter != wlr::Thread::s_thread_cache.end()) thread = (*iter).second;
    if (thread) {
		thread->m_flag = 3;
    }
    return data;
}

void wlr::Thread::run(void* args)
{
	// 0.线程未创建, 1.线程就绪, 2.线程调用函数, 3.执行完毕, 4.线程取消
    if (!this->m_flag) {
		this->m_flag = 1;
        this->m_args = args;
        pthread_create(&this->m_thread, NULL, wlr::Thread::start, this);
    }
}

void wlr::Thread::join()
{
	// 线程create之后才能join
	if ((this->m_flag - 1 ^ 2 - this->m_flag) > 0) {
		pthread_join(this->m_thread, NULL);
		this->m_flag = 3;
	}
}

bool wlr::Thread::stop()
{
	int r = 1;
	if((this->m_flag - 1 ^ 2 - this->m_flag) > 0) {
		// 成功返回0, 否则其他, c++11 无法执行
		r = pthread_cancel(this->m_thread);
		this->m_flag = 4;
	}

    return !r;
}

bool wlr::Thread::active()
{ return (this->m_flag - 1 ^ 2 - this->m_flag) > 0; }

void wlr::Thread::interrupt()
{ this->m_exit = 1; }

bool wlr::Thread::isInterrupt()
{ return this->m_exit; }


wlr::Mutex wlr::Thread::s_mutex = wlr::Mutex();
std::map<pid_t, wlr::Thread*> wlr::Thread::s_thread_cache = wlr::Thread::init();
std::map<pid_t, wlr::Thread*> wlr::Thread::init()
{
	std::map<pid_t, wlr::Thread*> thread_cache;
	wlr::Thread* main_thread = new wlr::Thread(NULL, "main");
	main_thread->m_flag = 2;
	main_thread->m_id = wlr::threadId();
	thread_cache.insert(std::pair<pid_t, wlr::Thread*>(main_thread->m_id, main_thread));
	printf("main thread start up, process id = %d\n", main_thread->m_id);
	return thread_cache;
}

void wlr::Thread::sleep(int millisecond)
{
	if (millisecond > 0)
		usleep(1000 * millisecond);
}

wlr::Thread* wlr::Thread::getCurrentThread(pid_t thread_id)
{
    // 通过线程pid, 从缓存中获取当前线程
    wlr::Thread *t = NULL;
	if (thread_id == -1)
		thread_id = wlr::threadId();

	std::map<pid_t, Thread*>::iterator iter = wlr::Thread::s_thread_cache.find(thread_id);
    if (iter != wlr::Thread::s_thread_cache.end()) t = (*iter).second;
    else LOG_WARN("not find thread %d.\n", thread_id);
    return t;
}

void wlr::Thread::printStack(int depth, std::vector<std::string> *out)
{
    int status = 0;
	size_t size = 0;
    void* buffer[depth];
    int len = backtrace(buffer, depth);
    //backtrace_symbols_fd(buffer, len, 1);
    char** buf =  backtrace_symbols(buffer, len);

    for (int i = 0; i < len; i++) {
		int index = 0;
		std::string str;
		char temp[1024];
		char address[128];
		int begin0 = false, index0 = 0;
		int begin1 = false, index1 = 0;
		for (; index < 1023; index++) {
			if (buf[i][index] == '\0') break;
			if (!begin1) {
				if (buf[i][index] == '+') {
					begin0 = false;
					temp[index0++] = '\0';
					continue;
				} else if (begin0) {
					temp[index0++] = buf[i][index];
					continue;
				} else if (buf[i][index] == '(') {
					begin0 = true;
					continue;
				}
			}

			if (!begin0) {
				if (buf[i][index] == ']') {
					address[index1++] = ']';
					address[index1++] = '\0';
					break;
				} else if (begin1 && index1 < 126) {
					address[index1++] = buf[i][index];
				} else if (buf[i][index] == '[') {
					address[index1++] = '[';
					begin1 = true;
				}
			}
		}
		// sscanf(buf[i], "%*[^(]%*[^_]%1023[^)+]", temp);
		// printf("%s, %d, %s, %d\n", address, index1, temp, index0);
		char* func_name = abi::__cxa_demangle(temp, NULL, &size, &status);
		// printf("func_name = %s, len = %d\n", func_name, size);
		if (func_name) {
			str.append(address, index1 -1).append(" ").append(func_name);
			free(func_name);
		} else {
			str.append(buf[i], index + 1);
		}
		
		if (out)
            (*out).push_back(str);
		else	
            printf("stack[%d] = %s\n", i, str.c_str());
	}

    free(buf);
}




wlr::ThreadFuture::ThreadFuture(intptr_t task_id)
	: m_task_id(task_id)
{
    wlr::ThreadFuture::s_map_mutex.lock();
    wlr::ThreadFuture::s_futute_map.insert(
		std::pair<intptr_t, std::pair<bool, void*> >(task_id, std::pair<bool, void*>(false, NULL)));
    wlr::ThreadFuture::s_map_mutex.unlock();
}

wlr::ThreadFuture::~ThreadFuture()
{
	wlr::ThreadFuture::s_map_mutex.lock();
    wlr::ThreadFuture::s_futute_map.erase(this->m_task_id);
    wlr::ThreadFuture::s_map_mutex.unlock();
}

bool wlr::ThreadFuture::isDone()
{
	auto iter = wlr::ThreadFuture::s_futute_map.find(this->m_task_id);
    return iter != wlr::ThreadFuture::s_futute_map.end() && (*iter).second.first;
}

void* wlr::ThreadFuture::get(unsigned int timeout_ms)
{
    int ms = static_cast<int>(timeout_ms);
    do
    {
		wlr::Thread::sleep(500);
        ms -= 500;
    } while(ms > 0 && !this->isDone());

    auto iter = wlr::ThreadFuture::s_futute_map.find(this->m_task_id);
    if (iter != wlr::ThreadFuture::s_futute_map.end())
        return (*iter).second.second;
    return NULL;
}

void wlr::ThreadFuture::set(intptr_t task_id, void* data)
{
    wlr::ThreadFuture::s_map_mutex.lock();
    auto iter = wlr::ThreadFuture::s_futute_map.find(task_id);
    if (iter != wlr::ThreadFuture::s_futute_map.end()) {
		(*iter).second.first = true;
		(*iter).second.second = data;
	}
    wlr::ThreadFuture::s_map_mutex.unlock();
}

wlr::Mutex wlr::ThreadFuture::s_map_mutex = wlr::Mutex();
std::map<intptr_t, std::pair<bool, void*> > wlr::ThreadFuture::s_futute_map = std::map<intptr_t, std::pair<bool, void*> >();



wlr::Runnable::Runnable(void* args)
	: m_args(args)
{}

void* wlr::Runnable::args()
{ return this->m_args; }

wlr::Runnable::~Runnable()
{
	// POOL_DEBUG("wlr::Runnable::~Runnable()\n");
}


wlr::ThreadPool::ThreadPool(int t_min, int t_max, int task_capacity) throw(ThreadException)
{
    if (t_min < 1) W_THROW(ThreadException, "the minimum thread value connot less than 1.");
    if (task_capacity < 1) W_THROW(ThreadException, "the task capacity connot less than 1.");
    if (t_max < t_min) t_max = t_min;

    this->m_min_t = t_min;
    this->m_max_t = t_max;
    this->m_live_t = t_min;
    this->m_exit = 0;
    this->m_pool_shutdown = 0;

    do {
		// 创建任务队列
		try {
			this->m_task_queue = new FifoQueue<wlr::ThreadPool::Task*>(task_capacity);
			POOL_DEBUG("init thread pool task queue done, queue capacity %d\n", this->m_task_queue->length());
		} catch(char* msg) {
			POOL_ERROR("thread task queue error. %s\n", msg);
            this->m_pool_shutdown = 1;
            W_THROW(ThreadException, "thread task queue error.");
            break;
		}

        // 初始化管理线程
        this->m_master_thread = new wlr::Thread(wlr::ThreadPool::master, "pool_thread_master");
        this->m_master_thread->run(this);

		// 初始化工作线程队列
        this->m_worker_threads = new wlr::Thread*[this->m_max_t];
        for (int i = 0; i < this->m_max_t; i++) 
		{
			if (i < t_min) {
                std::stringstream ss;
				ss << "pool_thread_worker_" << i;
				this->m_worker_threads[i] = new wlr::Thread(wlr::ThreadPool::worker, ss.str());
				this->m_worker_threads[i]->run(this);
				continue;
			}
			this->m_worker_threads[i] = NULL;
        }

		POOL_DEBUG("init thread pool done, min thread %d, max thread %d\n", this->m_min_t, this->m_max_t);
    } while (0);
}

wlr::ThreadPool::~ThreadPool()
{
    if (!this->isShutdown()){
		this->shutdown();
    }
    POOL_DEBUG("delete thread pool.\n");
}

wlr::Future* wlr::ThreadPool::submit(Task* task, OverflowPolicy overflow_policy) throw(ThreadException)
{
    do {
        if (this->isShutdown())
		{
            delete task;
			W_THROW(ThreadException, "thread pool is shutdown or task queue is full.\n");
			return NULL;
		}

		// 任务超出存活线程数*4的前提下创建新线程
        if (this->m_task_queue->size() >= (this->m_live_t << 2))
        {
            this->m_thread_mutex.lock();
            if (this->m_live_t < this->m_max_t) {
                for (int i = 0; i < this->m_max_t; i++)
                {
                    if (!this->m_worker_threads[i]) {
                        std::stringstream ss;
                        ss << "pool_thread_worker_" << i;
                        this->m_worker_threads[i] = new wlr::Thread(wlr::ThreadPool::worker, ss.str());
                        this->m_worker_threads[i]->run(this);
                        this->m_live_t++;
						POOL_DEBUG("---添加新线程索引为%d\n", i);
                        break;
                    }
                }
            }
			this->m_thread_mutex.unlock();
			break;
		} 
		else break;
		return NULL;
    } while (0);
        
    // 任务队列已满则清除任务, 否则添加到队列并发送信号
	bool r = this->m_task_queue->push(task);
	POOL_DEBUG("push task bool %d, queue use size %d\n", r, this->m_task_queue->size());
	if (!r) 
	{
		wlr::Thread* t;
		switch(overflow_policy)
		{
		case NEW_THREAD:
			t = new wlr::Thread(task->m_task, "pool_submit_new_thread");
			t->run(task->m_args);
			t->join();
			return NULL;
		case DISCARD_ONLY:
			W_THROW(ThreadException, "thread pool task queue is full.");
            return NULL;
		default:
			delete task;
			W_THROW(ThreadException, "thread pool task queue is full.");
			return NULL;
		}
	};

    this->m_task_sem.post();
	return new wlr::ThreadFuture((intptr_t)task);
}

wlr::Future* wlr::ThreadPool::submit(wlr::Runnable* runnable, OverflowPolicy overflow_policy) throw(ThreadException)
{
	wlr::ThreadPool::Task* task = new wlr::ThreadPool::Task;
	task->m_runnable = runnable;
	return this->submit(task, overflow_policy);
}

wlr::Future* wlr::ThreadPool::submit(void* (*func)(void*), void* args, OverflowPolicy overflow_policy) throw(ThreadException)
{
    wlr::ThreadPool::Task* task = new wlr::ThreadPool::Task;
    task->m_task = func;
	task->m_args = args;
    return this->submit(task, overflow_policy);
}

void wlr::ThreadPool::shutdown()
{
    this->m_pool_shutdown = 1;
    this->m_master_thread->join();
    delete this->m_master_thread;

    delete this->m_task_queue;
    for (int i = 0; i < this->m_max_t; i++)
        if (this->m_worker_threads[i]) delete this->m_worker_threads[i];
    delete[] this->m_worker_threads; 
}

bool wlr::ThreadPool::isShutdown()
{return this->m_pool_shutdown == 1;}

void* wlr::ThreadPool::master(void* pool)
{
    wlr::ThreadPool *tp = (wlr::ThreadPool*)pool;
    
    while (1)
    {
		if (tp->isShutdown())
		{
			// 结束工作线程
			for (int i = 0; i < tp->m_max_t; i++)
                if (tp->m_worker_threads[i]) tp->m_worker_threads[i]->interrupt();

			tp->m_exit = tp->m_live_t;
			for (int i = 0; i < tp->m_live_t; i++)
			{
				tp->m_task_sem.post();
			}
	    
			// 清除未执行完的任务
			std::pair<bool, wlr::ThreadPool::Task*> task_pair = tp->m_task_queue->pop();
			while(task_pair.first)
			{
				if (task_pair.second->m_runnable) {
					delete task_pair.second->m_runnable;
				}
				delete task_pair.second;
				task_pair = tp->m_task_queue->pop();
			}

			sleep(3);
			break;	    
		}

		// 任务紧张则在最大线程数的前提下创建新线程
		if (static_cast<int>(tp->m_task_queue->length() >> 1) < tp->m_task_queue->size())
		{
			POOL_DEBUG("task tension, create new thread\n");
			tp->m_thread_mutex.lock();
			if (tp->m_live_t < tp->m_max_t) {
				for (int i = 0; i < tp->m_max_t; i++)
				{
					if (!tp->m_worker_threads[i]) {
						std::stringstream ss;
						ss << "pool_thread_worker_" << i;
						tp->m_worker_threads[i] = new wlr::Thread(wlr::ThreadPool::worker, ss.str());
						tp->m_worker_threads[i]->run(tp);
						tp->m_live_t++;
						POOL_DEBUG("---新线程索引为%d\n", i);
						break;
					}
				} 
			}
			tp->m_thread_mutex.unlock();
		}

		// 清理待释放内存线程
		tp->m_thread_mutex.lock();
		for (int i = 0; i < tp->m_max_t; i++)
		{
			if (tp->m_worker_threads[i] && !tp->m_worker_threads[i]->active()) {
				delete tp->m_worker_threads[i];
				tp->m_worker_threads[i] = NULL;
				POOL_DEBUG("---清理索引为%d\n", i);
			}
		}
		tp->m_thread_mutex.unlock();

		// 如果任务空闲则释放多余的线程
        if (tp->m_task_queue->size() < tp->m_live_t && tp->m_live_t > tp->m_min_t)
        {
			POOL_DEBUG("master start cleaning up free threads, clean num = %d\n", tp->m_exit + 1);
			tp->m_thread_mutex.lock();
			//int live = tp->m_live_t;
			//for (int i = tp->m_min_t; i < live; i++) {
			tp->m_exit++;
			tp->m_task_sem.post();
			tp->m_live_t--;
			//}
			tp->m_thread_mutex.unlock();
        }

		sleep(3);
    }

    return NULL;
}

void* wlr::ThreadPool::worker(void* pool)
{
    wlr::ThreadPool *tp = (wlr::ThreadPool*)pool;
    while (1)
    {
		// 等待任务
		if (!tp->isShutdown()) tp->m_task_sem.wait();	

		// 判断是否需要退出
		wlr::Thread *self = wlr::Thread::getCurrentThread();
		if (tp->isShutdown() || tp->m_exit > 0 || self->isInterrupt())
		{
			tp->m_thread_mutex.lock();
			for (int i = 0; i < tp->m_max_t; i++)
			{
				if (tp->m_worker_threads[i] && tp->m_worker_threads[i] == self){
					tp->m_exit--;
					POOL_DEBUG("+++待清理索引为%d\n", i);
					tp->m_thread_mutex.unlock();
					return NULL;
				}
			}
			tp->m_thread_mutex.unlock();	
	        if (tp->isShutdown()) return NULL;
        }
	
		// 获取任务并执行
        std::pair<bool, wlr::ThreadPool::Task*> task_pair = tp->m_task_queue->pop();
		wlr::ThreadPool::Task* task = task_pair.second;
        if (task_pair.first && task) {
			void* return_data = NULL;
			intptr_t task_id = (intptr_t)task;
			try {
				if (task->m_task) return_data = task->m_task(task->m_args);
				if (task->m_runnable) {
					return_data = task->m_runnable->run(task->m_runnable->args());
					delete task->m_runnable;
					task->m_runnable = NULL;
				}
			} catch(wlr::Exception e) {
				POOL_ERROR("%s\n%s", e.message().c_str(), e.stackString().c_str());
			} catch(...) {
				wlr::Thread::printStack(10);
				if (task->m_runnable) delete task->m_runnable;
			}
			wlr::ThreadFuture::set(task_id, return_data);
	        delete task;
	    }
    }

    return NULL;
}

