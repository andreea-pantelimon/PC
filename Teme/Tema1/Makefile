all: send recv

link_emulator/lib.o:
	$(MAKE) -C link_emulator

send: send.o general_utils.o send_utils.o link_emulator/lib.o
	gcc -g $^ -o send

recv: recv.o general_utils.o recv_utils.o link_emulator/lib.o
	gcc -g $^ -o recv

.c.o:
	gcc -g -c $? -Wall -Wextra -Werror -std=c99

clean:
	$(MAKE) -C link_emulator clean
	-rm -f *.o send recv

