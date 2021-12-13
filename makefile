CC=gcc
CFLAGS=-g -Wall -pthread
EXE=server client destroy

all: $(EXE)

server: server.o memory.o
	gcc -g -Wall -pthread server.o memory.o -o server

client: client.o memory.o
	gcc -g -Wall -pthread client.o memory.o -o client

destroy: destroy.o memory.o
	gcc -g -Wall -pthread destroy.o memory.o -o destroy

server.o: server.c
	gcc -g -Wall -pthread -c server.c

client.o: client.c
	gcc -g -Wall -pthread -c client.c

destroy.o: destroy.c
	gcc -g -Wall -pthread -c destroy.c

memory.o: memory.c
	gcc -g -Wall -pthread -c memory.c

clean:
	rm server client destroy *.o