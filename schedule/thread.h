#ifndef __WLR__THREAD__H
#define __WLR__THREAD__H


#include <pthread.h>
#include <string.h>
#include <sstream>
#include <vector>
#include <map>

#include "wlock.h"
#include "future.h"
#include "../utils/logger.h"
#include "../utils/exception.h"
#include "../structure/fifoqueue.h"

#define POOL_DEBUG(format, ...) LOG_FORMAT(wlr::Logger::DEBUG, "thread_pool.log", format, ##__VA_ARGS__)
#define POOL_ERROR(format, ...) LOG_FORMAT(wlr::Logger::ERROR, "thread_pool.log", format, ##__VA_ARGS__)


namespace wlr
{


class Thread
{
public:
    Thread(void* (*func)(void*), std::string name = "");
    //Thread(const Thread&) = delete;
    //Thread operator= (const Thread&) = delete;
    ~Thread();

    pid_t getId() const;         // 获取线程pid, run之后才会生成
    std::string getName() const; // 获取线程名

    void run(void* args);        // 创建执行线程
    void join();                 // 阻塞等待线程执行完成
    bool stop();				 // 终止线程
	bool active();				 // 线程是否存活
	void interrupt();			 // 中断线程
	bool isInterrupt();			 // 是否需要中断线程

private:
	char m_exit;				 // 线程是否退出
    char m_flag;                 // 线程执行标志
    pid_t m_id;                  // 线程pid
    std::string m_name;          // 线程名
    pthread_t m_thread;          // 线程标志

    void *m_args;                // 执行函数入参
    void* (*m_func)(void*);      // 执行函数

    static void* start(void*);   // 执行函数前后置处理
    static wlr::Mutex s_mutex;   // 缓存操作互斥锁
	static std::map<pid_t, Thread*> init();
    static std::map<pid_t, Thread*> s_thread_cache;

    Thread(const Thread&){}
    Thread operator= (const Thread&){}
public:
	static void sleep(int millisecond);												// 当前线程进入睡眠, 单位为毫秒
    static Thread* getCurrentThread(pid_t thread_id = -1);							// 获取当前线程
	static void printStack(int depth = 100, std::vector<std::string> *out = NULL);	// 打印线程栈信息
};



// 由线程池返回
class ThreadFuture : public Future
{
public:
    ThreadFuture(intptr_t task_id);
    virtual bool isDone() override;
    virtual void* get(unsigned int timeout_ms = INT_MAX) override;
    virtual ~ThreadFuture();

	static void set(intptr_t task_id, void* data);
private:
	intptr_t m_task_id;
    static wlr::Mutex s_map_mutex;
    static std::map<intptr_t, std::pair<bool, void*> > s_futute_map;
};



class Runnable
{
public:
	explicit Runnable(void* args = 0);
	void* args();
    virtual void* run(void* args) = 0;
	virtual ~Runnable();
private:
	void* m_args;
};



class FunctionRunnable : public Runnable
{
public:
	template<typename F>
	explicit FunctionRunnable(F func)
		: FunctionRunnable(func, NULL)
	{}

	template<typename F>
    explicit FunctionRunnable(F func, void* args)
        : Runnable(args)
        , m_func((void* (*)(void*))func)
    {}

    virtual void* run(void* args) override
    { return this->m_func(args); }

    virtual ~FunctionRunnable() override {}

private:
	void* (*m_func)(void*);
};



class ThreadPool
{
public:
    typedef struct ThreadTask
    {
        void* m_args = NULL;
        void* (*m_task)(void*) = 0;
		wlr::Runnable* m_runnable = NULL;
    } Task;

	typedef enum Policy
	{
		// 提交任务超出队列容量策略
		DISCARD_DELETE,				// 丢弃任务并delete. 默认抛出wlr::ThreadException异常
		DISCARD_ONLY,				// 仅丢弃任务. 默认抛出wlr::ThreadException异常
		NEW_THREAD,					// 创建新线程执行
		
	} OverflowPolicy;

	/*
	* @t_min			最小线程数
	* @t_max			最大线程数	
	* @task_capacity	任务队列最大容量, 溢出默认策略丢弃任务并delete
	*/
    ThreadPool(int t_min, int t_max, int task_capacity = 1024) throw(wlr::ThreadException);
    ~ThreadPool();

private:
    int m_min_t;	// 线程最小数
    int m_max_t;	// 线程最大数
    int m_live_t;	// 线程存活数

    int m_exit;							// 线程退出标志
	int m_pool_shutdown;				// 线程池结束标志

	wlr::FifoQueue<Task*>* m_task_queue;//任务队列
    wlr::Thread** m_worker_threads;		// 工作线程队列
    wlr::Thread* m_master_thread;		// 管理线程

    wlr::Mutex m_thread_mutex;			// 线程创建回收互斥锁
    wlr::Semaphore m_task_sem;			// 任务添加通知信号量

private:
	/*
	* 将任务提交到线程池执行
	* @ task				执行任务
	* @ overflow_policy		任务溢出策略
	* @ return				任务执行结果Future, 需手动清理Future及get()返回值
	*/
    wlr::Future* submit(Task* task, OverflowPolicy overflow_policy) throw(wlr::ThreadException);

public:
    wlr::Future* submit(void* (*func)(void*), void* args, OverflowPolicy overflow_policy = DISCARD_DELETE) throw(wlr::ThreadException);
    wlr::Future* submit(wlr::Runnable* runnable, OverflowPolicy overflow_policy = DISCARD_DELETE) throw(wlr::ThreadException);
	

    void shutdown();		// 关闭线程池, 析构默认调用
    bool isShutdown();		// 线程池是否关闭, 关闭标志=1
    
private:
    static void* worker(void*); 	// 工作线程执行函数
    static void* master(void*);		// 管理线程执行函数
};



}



#endif
