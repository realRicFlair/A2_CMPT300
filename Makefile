CFLAGS = -std=gnu17 -Wall -Werror -g

all: build

build:
	gcc $(CFLAGS) main.c list.o queue.c keyboard.c send.c display.c receive.c -o s-talk -lpthread
	
clean:
	rm -f s-talk