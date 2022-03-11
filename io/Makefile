CC=g++ -std=c++11
ARGS=buf.o socket.o channel.o niosocketchannel.o defaultchannelpipeline.o

compile : $(ARGS)

buf.o : buf.cc buf.h
	$(CC) -c buf.cc -o buf.o -I./ -I../utils/

socket.o : socket.cc socket.h
	$(CC) -c socket.cc -o socket.o -I./

channel.o : channel.cc channel.h ichannelpipeline.h
	$(CC) -c channel.cc -o channel.o -I./

niosocketchannel.o : niosocketchannel.cc niosocketchannel.h channel.h defaultchannelpipeline.h
	$(CC) -c niosocketchannel.cc -o niosocketchannel.o -I./

defaultchannelpipeline.o : defaultchannelpipeline.cc defaultchannelpipeline.h ../handle/channelcontext.h ichannelpipeline.h
	$(CC) -c defaultchannelpipeline.cc -o defaultchannelpipeline.o -I./ -I../handle/

.PHONY: clean
clean :
	rm -rf $(ARGS)
