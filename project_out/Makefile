all: create liblinux_sys.a liblinux_sys.so

liblinux_sys.so: safe_shm.o loop_queue.o
	gcc -shared $^ -o lib/$@

liblinux_sys.a: safe_shm.o loop_queue.o
	ar rcs lib/$@ $^

safe_shm.o: ../linux_safe_shm/safe_shm.c
	gcc -fpic -c $< -o $@

loop_queue.o: ../linux_loop_queue/loop_queue.c
	gcc -fpic -c $< -o $@

.PHONY: create
create:
	cp ../linux_safe_shm/*.h include/
	cp ../linux_loop_queue/*.h include/

.PHONY: clean
clean:
	rm  -f *.o lib/*
