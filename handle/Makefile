CC=g++ -std=c++11
ARGS=handler.o channelcontext.o initializerchannelhandler.o messagehandler.o channelcontexthandler.o

compile : $(ARGS)

handler.o : handler.cc handler.h ../io/buf.h ../io/channel.h
	$(CC) -c handler.cc -o handler.o -I./ -I../io/

channelcontext.o : channelcontext.cc channelcontext.h inboundchannelhandler.h outboundchannelhandler.h ../schedule/thread.h
	$(CC) -c channelcontext.cc -o channelcontext.o -I./ -I/../schedule/

initializerchannelhandler.o : initializerchannelhandler.cc initializerchannelhandler.h channelcontext.o
	$(CC) -c initializerchannelhandler.cc -o initializerchannelhandler.o -I./ -I../utils/

messagehandler.o : messagehandler.h messagehandler.cc bytetransformhandler.h
	$(CC) -c messagehandler.cc -o messagehandler.o -I./

channelcontexthandler.o :  channelcontexthandler.cc channelcontexthandler.h channelcontext.o ../schedule/threadlocal.h
	$(CC) -c channelcontexthandler.cc -o channelcontexthandler.o -I./ -I../schedule/

.PHONY: clean
clean :
	rm -rf $(ARGS)
