all: Demo

Demo: Demo.c ../loop_queue.c
	g++ Demo.c ../loop_queue.c -I ../ -lpthread -o Demo

.PHONY:clean
clean:
	rm -f Demo

