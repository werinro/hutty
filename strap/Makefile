CC=g++ -std=c++11
ARGS=channelfuture.o bootstrap.o

compile : $(ARGS)

channelfuture.o : channelfuture.cc channelfuture.h ../structure/maps.h
	$(CC) -c channelfuture.cc -o channelfuture.o -I./ -I../utils/

bootstrap.o : bootstrap.cc bootstrap.h ../io/ichannelpipeline.h ../handle/initializerchannelhandler.o ../utils/logger.h
	$(CC) -c bootstrap.cc -o bootstrap.o -I./ -I../handle/ -I../io/ -I../utils/

.PHONY: clean
clean :
	rm -rf $(ARGS)
