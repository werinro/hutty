EXTRA = -rdynamic -pthread
CC = g++ -std=c++11 $(EXTRA)
.PHONY: clean compile link

ABS_PATH := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
HEADERS_PATH = ./ ../../handle/ ../../schedule/ ../../io/ ../../utils/ ../../strap/

LINK_ARGS = $(wildcard $(addsuffix *.o, $(addprefix $(ABS_PATH), $(HEADERS_PATH))))
# LINK_ARGS := $(patsubst %.cc, %.o, $(LINK_ARGS))
LINK_ARGS += message.cc

TARGET = message
COMPILE_ARGS = MessageInitializerChannelHandler.o

$(TARGET) : compile
	make link

compile : $(COMPILE_ARGS)
	cd ../../utils/ && make
	cd ../../io/ && make
	cd ../../handle/ && make
	cd ../../strap/ && make
	cd ../../schedule/ && make

link :
	$(CC) $(LINK_ARGS) -o $(TARGET) $(addprefix -I, $(HEADERS_PATH))

clean :
	rm -rf $(COMPILE_ARGS)
	cd ../../utils/ && make clean
	cd ../../io/ && make clean
	cd ../../handle/ && make clean
	cd ../../strap/ && make clean
	cd ../../schedule/ && make clean
	cd $(ABS_PATH)


MessageInitializerChannelHandler.o : MessageInitializerChannelHandler.cc MessageInitializerChannelHandler.h ../../handle/handler.h
	$(CC) -c MessageInitializerChannelHandler.cc -o MessageInitializerChannelHandler.o -I./ -I../../handle/

