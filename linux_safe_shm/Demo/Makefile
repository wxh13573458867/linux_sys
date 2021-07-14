all:Demo_read	Demo_write

Demo_read: Demo_read.c ../linux_sys.c
	gcc Demo_read.c ../linux_sys.c -I ../ -o Demo_read

Demo_write: Demo_write.c ../linux_sys.c
	gcc Demo_write.c ../linux_sys.c -I ../ -o Demo_write

clean:
	rm Demo_read Demo_write
