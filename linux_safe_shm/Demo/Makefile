all:	Demo_read	Demo_write

Demo_read: Demo_read.c ../safe_shm.c
	gcc Demo_read.c ../safe_shm.c -I ../ -o Demo_read

Demo_write: Demo_write.c ../safe_shm.c
	gcc Demo_write.c ../safe_shm.c -I ../ -o Demo_write

.PHONY:clean
clean:
	rm -f Demo_read Demo_write
