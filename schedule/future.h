#ifndef __WLR__FUTURE__H
#define __WLR__FUTURE__H


#ifndef INT_MAX
#define INT_MAX 0x7FFFFFFF
#endif


namespace wlr
{


class Future
{
public:
	virtual ~Future() {}
	virtual bool isDone() = 0;
	virtual void* get(unsigned int timeout_ms = INT_MAX) = 0;
};


}


#endif
