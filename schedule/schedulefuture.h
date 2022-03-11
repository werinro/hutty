#ifndef __WLR__SCHEDULEFUTURE__H
#define __WLR__SCHEDULEFUTURE__H


#include "future.h"


namespace wlr
{


class ScheduleFuture : public Future
{
public:
	virtual void schedule(unsigned int millisecond);
	virtual void* get(unsigned int);
	virtual bool isDone();

private:
	char m_done = 0;
};



}


#endif
