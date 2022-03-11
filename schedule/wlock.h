#ifndef __WLR__WLOCK__H
#define __WLR__WLOKC__H

#pragma once


#include <semaphore.h>
#include <pthread.h>


namespace wlr
{


// 互斥锁
class Mutex
{
public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();

    void wait();
    void signal(); // 唤醒单个wait m_cond线程
	void broadcast(); // 唤醒所有wait m_cond线程
private:
    pthread_cond_t m_cond;
    pthread_mutex_t m_mutex;
};


// 读写锁
class ReadWriteLock
{
public:
    ReadWriteLock();
    ~ReadWriteLock();

    void readLock();  // 锁定读取操作
    void writeLock(); // 锁定写入操作
    void unlock();
private:
    pthread_rwlock_t m_lock;
};


// 自旋锁
class SpinLock
{
public:
    SpinLock();
    ~SpinLock();

    void lock();
    void unlock();
private:
    pthread_spinlock_t m_lock;
};


// 信号量
class Semaphore
{
public:
    Semaphore();
    ~Semaphore();

    void wait(); // 等待信号量>0
    void post(); // 信号量+1
private:
    sem_t m_sem;
};


}


#endif
