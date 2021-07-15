all: Demo

Demo: Demo.c ../loop_queue.c
	gcc Demo.c ../loop_queue.c -I ../ -lpthread -o Demo

.PHONY:clean
clean:
	rm -f Demo

