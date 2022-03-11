#include "channelfuture.h"



void wlr::ChannelFuture::addListener(wlr::FutureListener* listener)
{ this->m_fl_list.push_back(listener); }

void wlr::ChannelFuture::delListener(wlr::FutureListener* listener)
{ this->m_fl_list.remove(listener); }

#define W_ALL(func) \
	wlr::foreach(this->m_fl_list, [this, mg](wlr::FutureListener* listener, size_t index) { \
		listener->func(this, mg); \
	});

void wlr::ChannelFuture::init(std::string mg)
{ 
	W_ALL(onInit) 
}

void wlr::ChannelFuture::error(std::string mg)
{ 
	W_ALL(onError) 
}

void wlr::ChannelFuture::done(std::string mg)
{ 
	W_ALL(onDone) 
}

wlr::ChannelFuture::~ChannelFuture()
{
	wlr::foreach(this->m_fl_list, [](wlr::FutureListener* listener, size_t index) {
		if (listener) delete listener;
    });	
	this->m_fl_list.clear();
}

#undef W_ALL



#define W_ON(state) \
	this->m_func(state, future, mg);

void wlr::LambdaFutureListener::onInit(wlr::ChannelFuture* future, std::string mg)
{
	W_ON(wlr::LambdaFutureListener::INIT)
}

void wlr::LambdaFutureListener::onError(wlr::ChannelFuture* future, std::string mg)
{
	W_ON(wlr::LambdaFutureListener::ERROR)
}

void wlr::LambdaFutureListener::onDone(wlr::ChannelFuture* future, std::string mg)
{
	W_ON(wlr::LambdaFutureListener::DONE)
}

#undef W_ON
