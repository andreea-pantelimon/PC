CC=g++ -std=c++11 -Wall -Wextra

all: build

build: server subscriber

server: server_utils.o server.o
	$(CC) $^ -o $@

server.o: server.cpp
	$(CC) $^ -c

server_utils.o: server_utils.cpp
	$(CC) $^ -c

subscriber: subscriber_utils.o subscriber.o
	$(CC) $^ -o $@

subscriber_utils.o: subscriber_utils.cpp
	$(CC) $^ -c

subscriber.o: subscriber.cpp
	$(CC) $^ -c

clean:
	rm *.o server subscriber
