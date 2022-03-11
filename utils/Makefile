CC=g++ -std=c++11
ARGS=logger.o config.o exception.o

compile : $(ARGS)

logger.o : logger.cc logger.h
	$(CC) -c logger.cc -o logger.o -I./ -DLOG_LEVEL=W_DEBUG -DLOG_DIR=\"./logs/\"

config.o : config.cc config.h
	$(CC) -c config.cc -o config.o -I./ -DCONFIG_FILE_PATH=\"./config.ini\"

exception.o : exception.cc exception.h
	$(CC) -c exception.cc -o exception.o -I./

.PHONY: clean
clean :
	rm -rf $(ARGS)
