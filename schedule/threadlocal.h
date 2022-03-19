#ifndef __WLR__THREADLOCAL__H
#define __WLR__THREADLOCAL__H


#include "../structure/maps.h"
#include "../utils/constant.h"


namespace wlr
{



template<class Value, Value* v_default = (Value*)NULL>
class ThreadLocal
{
public:
	ThreadLocal() = default;

	inline Value* get()
	{ 
		auto iter = this->m_cache.find(wlr::threadId());
		if (iter != this->m_cache.end()) return &(*iter).second;
		return v_default; 
	}

	inline void set(Value value)
	{ this->m_cache[wlr::threadId()] = value; }

	~ThreadLocal() = default;


private:
	std::map<int, Value> m_cache;
};




}



#endif
