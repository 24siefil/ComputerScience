CC = gcc

ladders: ladders.c backend.o backend.h
	${CC} -g -o ladders ladders.c backend.o

backend.o: backend.c
	${CC} -g -c backend.c

clean:
	rm ladders ladders_demo *.o *.aux *.log *.zip
