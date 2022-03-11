#include "wlock.h"



wlr::Mutex::Mutex()
{
    this->m_cond = PTHREAD_COND_INITIALIZER;
    pthread_mutex_init(&this->m_mutex, NULL);
}

wlr::Mutex::~Mutex()
{
    pthread_mutex_destroy(&this->m_mutex);
    pthread_cond_destroy(&this->m_cond);
}

void wlr::Mutex::lock()
{pthread_mutex_lock(&this->m_mutex);}

void wlr::Mutex::unlock()
{pthread_mutex_unlock(&this->m_mutex);}

void wlr::Mutex::wait()
{pthread_cond_wait(&this->m_cond, &this->m_mutex);}

void wlr::Mutex::signal()
{pthread_cond_signal(&this->m_cond);}

void wlr::Mutex::broadcast()
{pthread_cond_broadcast(&this->m_cond);}



wlr::ReadWriteLock::ReadWriteLock()
{
    pthread_rwlock_init(&this->m_lock, NULL);
}

wlr::ReadWriteLock::~ReadWriteLock()
{
    pthread_rwlock_destroy(&this->m_lock);
}

void wlr::ReadWriteLock::readLock()
{pthread_rwlock_rdlock(&this->m_lock);}

void wlr::ReadWriteLock::writeLock()
{pthread_rwlock_wrlock(&this->m_lock);}

void wlr::ReadWriteLock::unlock()
{pthread_rwlock_unlock(&this->m_lock);}



wlr::SpinLock::SpinLock()
{
    pthread_spin_init(&this->m_lock, 0);
}

wlr::SpinLock::~SpinLock()
{
    pthread_spin_destroy(&this->m_lock);
}

void wlr::SpinLock::lock()
{pthread_spin_lock(&this->m_lock);}

void wlr::SpinLock::unlock()
{pthread_spin_unlock(&this->m_lock);}



wlr::Semaphore::Semaphore()
{
    sem_init(&this->m_sem, 0, 0);
}

wlr::Semaphore::~Semaphore()
{
    sem_destroy(&this->m_sem);
}

void wlr::Semaphore::wait()
{sem_wait(&this->m_sem);}

void wlr::Semaphore::post()
{sem_post(&this->m_sem);}

