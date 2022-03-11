#ifndef __WLR__CHANNELFUTURE__H
#define __WLR__CHANNELFUTURE__H


#include <string.h>
#include <list>
#include "../structure/maps.h"


namespace wlr
{


class ChannelFuture;
class FutureListener
{
public:
	virtual void onInit(wlr::ChannelFuture* future, std::string mg = "") = 0;
	virtual void onError(wlr::ChannelFuture* future, std::string mg = "") = 0;
	virtual void onDone(wlr::ChannelFuture* future, std::string mg = "") = 0;
	virtual ~FutureListener() {}
};


class ChannelFuture
{
public:
	void addListener(wlr::FutureListener* listener);
	void delListener(wlr::FutureListener* listener);
	void init(std::string mg);
	void error(std::string mg);
	void done(std::string mg);
	~ChannelFuture();

protected:
	std::list<FutureListener*> m_fl_list;
};


class LambdaFutureListener : public FutureListener
{
public:
	typedef enum State
	{
		INIT,
		ERROR,
		DONE,
	} State;
	
	template<typename F>
	explicit LambdaFutureListener(F f)
		: m_func((void(*)(wlr::LambdaFutureListener::State, wlr::ChannelFuture*, std::string))f)
	{}

	virtual void onInit(wlr::ChannelFuture* future, std::string mg = "");
    virtual void onError(wlr::ChannelFuture* future, std::string mg = "");
    virtual void onDone(wlr::ChannelFuture* future, std::string mg = "");

private:
	void (*m_func)(wlr::LambdaFutureListener::State, wlr::ChannelFuture*, std::string);
};



}


#endif
