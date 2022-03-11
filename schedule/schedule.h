#ifndef __WLR__SCHEDULE__H
#define __WLR__SCHEDULE__H


namespace wlr
{


class Schedule
{
public:
	virtual ~Schedule() {}
	virtual unsigned int waitMillisecond() = 0;
	virtual void* execute() = 0;
};


}


#endif
