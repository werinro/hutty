# -rdynamic(backtrace()栈函数名)
EXTRA=-rdynamic
CC=g++ -std=c++11 $(EXTRA)
ARGS=wlock.o thread.o threadeventloopgroup.o

compile : $(ARGS)
	
#$(CC) test.cc ../utils/logger.o ../utils/exception.o $(ARGS) -o test -pthread

threadeventloopgroup.o : threadeventloopgroup.cc threadeventloopgroup.h thread.h eventloopgroup.h
	$(CC) -c threadeventloopgroup.cc -o threadeventloopgroup.o -I./

wlock.o : wlock.cc wlock.h
	$(CC) -c wlock.cc -o wlock.o -I./

thread.o : thread.cc thread.h wlock.h ../utils/logger.h ../utils/exception.h
	$(CC) -c thread.cc -o thread.o -I./ -I../utils/ -pthread

logger.o : ../utils/logger.o
	cd ../utils/
	make logger.o

exception.o : ../utils/exception.o
	cd ../utils/
	make exception.o

.PHONY: clean
clean :
	rm -rf $(ARGS)
