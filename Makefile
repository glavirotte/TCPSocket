CC = gcc
OPT = -g
FLAGS = -lpthread

all: client serverThread

client: client.o
	$(CC) $(OPT) -o $@ $< $(FLAGS)

client.o: client.c
	$(CC) $(OPT) -o $@ -c $< $(FLAGS)

serverThread: serverThread.o linkedlist.o
	$(CC) $(OPT) -o $@ $^ $(FLAGS)

serverThread.o: serverThread.c linkedlist.c
	$(CC) $(OPT) -o $@ -c $< $(FLAGS)

linkedlist.o: linkedlist.c
	$(CC) $(OPT) -o $@ -c $< $(FLAGS)

clean:
	rm -rf *.o client serverThread