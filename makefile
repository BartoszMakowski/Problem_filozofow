CC=gcc
CFLAGS=-Wall

all: ./sem_shm/5fil.out ./sem/5fil.out

./sem_shm/5fil.out:
	$(CC) $(CFLAGS) ./sem_shm/main.c -o ./sem_shm/5fil.out
	
./sem/5fil.out:
	$(CC) $(CFLAGS) ./sem/main.c -o ./sem/5fil.out
