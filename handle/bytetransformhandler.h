#ifndef __WLR__BYTETRANSFORMHANDLER__H
#define __WLR__BYTETRANSFORMHANDLER__H


#include <list>
#include "inboundchannelhandler.h"
#include "outboundchannelhandler.h"
#include "../schedule/threadlocal.h"


namespace wlr
{


template<class Type>
class ByteTransformHandler : public IChannelInbound, public IChannelOutbound
{
public:

	/**
	*	Decode from in_buf to std::list<Type>
	*	@param chc 
	*	@param in_buf   channel read buf
	*	@param out_list decode type list
	*/
	virtual void channelReadHandler(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* in_buf, std::list<Type> *out_list) = 0;

	/**
	* Type handler
	* @param type_list copy out_list. if the <Type> dynamically memory, you need to free the memory
	*/ 
	virtual void handler(std::list<Type> type_list) = 0;
	
	/**
	*	Encode from std::list<Type> to out_buf
	*	@param chc
	*	@param in_buf  channel write buf
	*	@param in_list encode type list
	*/ 
	virtual void channelWriteHandler(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* out_buf, std::list<Type> in_list) = 0;

	explicit ByteTransformHandler() 
		: IChannelInbound()
		, IChannelOutbound() 
	{}

	virtual inline void channelRead(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) override final
    {
        std::list<Type> type_list;
        this->m_type_local.set(type_list);
        this->channelReadHandler(chc, buf, &type_list);
        this->handler(type_list);
    }

	virtual inline void channelWrite(wlr::ChannelHandlerContext* chc, wlr::ByteBuf* buf) override final
    { this->channelWriteHandler(chc, buf, *this->m_type_local.get()); }

	inline void write(Type type)
	{
		std::list<Type> *local = this->m_type_local.get();
		local->push_back(type);
	}

    inline void write(std::list<Type> type_list)
	{
		std::list<Type> *local = this->m_type_local.get();
		for (Type type: type_list) local->push_back(type);
	}

	virtual ~ByteTransformHandler() override {}
	
private:
	wlr::ThreadLocal<std::list<Type> > m_type_local;
};


}

#endif
