CC=gcc
CFLAGS=-Wall

client: client.o parson.o utils.o
	$(CC) $^ -o $@

client.o: client.c
	$(CC) $(CFLAGS) $< -c

utils.o: utils.c
	$(CC) $(CFLAGS) $< -c

parson.o: parson.c
	$(CC) $(CFLAGS) $< -c

run: client
	./client

clean:
	rm -f *.o client
