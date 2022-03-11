#ifndef __WLR__FIFOQUEUE__H
#define __WLR__FIFOQUEUE__H


#include <stdlib.h>
#include <pthread.h>

#include "../utils/constant.h"


namespace wlr
{


template<class Type, size_t d_l = 3>
class FifoQueue
{
public:
	explicit FifoQueue()
		: FifoQueue(d_l)
	{}

    explicit FifoQueue(size_t length)
        : m_length(length)
    {
        if (length > INT_MAX) throw "Maximum length cannot exceed INT_MAX!";
        size_t size = sizeof(Type) * length;
        this->m_data_array = (Type*)malloc(size);

		if (!this->m_data_array) throw "Queue malloc error!";
        pthread_mutex_init(&this->m_sync_mutex, NULL);
    }

    ~FifoQueue()
    {
        free(this->m_data_array);
        pthread_mutex_destroy(&this->m_sync_mutex);
    }

private:
    /*
    * 入队索引
    * @return 返回当前写入索引 array[index] = ?
	*/
    inline size_t push_index()
    {
        size_t index = -1;
        if (this->m_push_index < this->m_length) {
            index = this->m_push_index++;
        } else if (this->m_new_index < this->m_pop_index) {
            index = this->m_new_index++;
        }
        return index;
    }

    /*
    * 出队索引
	* @return 返回当前写入索引 array[index] = ?
    */
    inline size_t pop_index()
    {
        size_t index = -1;
        if (this->m_pop_index < this->m_push_index) {
            index = this->m_pop_index++;
        } else if (this->m_push_index >= this->m_length && this->m_new_index > 0) {
            this->m_push_index = this->m_new_index;
			this->m_pop_index = this->m_new_index = 0;
            index = this->m_pop_index++;
        }
        return index;
    }

public:
    /*
    * 数据入队
	* @return 是否入队成功
    */
    inline bool push(Type data)
    {
		bool r;
		pthread_mutex_lock(&this->m_sync_mutex);
        size_t index = this->push_index();
        if (r = (index != -1)) {
            this->m_data_array[index] = data;
        }

		pthread_mutex_unlock(&this->m_sync_mutex);
        return r;
    }

    /*
    * 数据出队, use if (pair.first) pair.second ?
    * @return 返回数据对, pair<是否获取成功, (Type)data>
    */
	inline std::pair<bool, Type> pop()
    {
		std::pair<bool, Type> pair;
		pair.first = false;
		pthread_mutex_lock(&this->m_sync_mutex);
        size_t index = this->pop_index();
        if (index != -1) {
			// ? 返回(Type*) = &this->m_data_array[index], 在线程并发情况下可能会在使用未释放的数组索引地址导致重复释放指针
			// ? 返回(Type) 非指针Type无法返回默认值
			// ? void pop(Type* data) & Type pop(Type default) { return default;} 待考虑
            pair = std::pair<bool, Type>(true, this->m_data_array[index]);
        }
		pthread_mutex_unlock(&this->m_sync_mutex);
        return pair;
    }


    /*
    * 返回已使用队列大小
    */
    inline size_t size()
    { return this->m_push_index - this->m_pop_index + this->m_new_index; }

    /*
    * 返回队列总长度
    */
    inline size_t length()
    { return this->m_length; }

private:
    size_t m_length;                // 数组长度
    size_t m_new_index = 0;         // 前置入队索引
    size_t m_push_index = 0;        // 后置入队索引
    size_t m_pop_index = 0;         // 出队索引
    pthread_mutex_t m_sync_mutex;   // 同步锁
    Type* m_data_array;             // 数据数组
};


}


#endif
