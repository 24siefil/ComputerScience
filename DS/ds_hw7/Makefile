CC = gcc

ladders: ladders.c backend.o backend.h path.o path.h
	${CC} -g -o ladders ladders.c backend.o path.o

backend.o: backend.c backend.h
	${CC} -g -c backend.c

path.o: path.c backend.h
	${CC} -g -c path.c

test: ladders
	./ladders < test.in > test.out
	diff ch_test.out test.out

clean:
	rm ladders ladders_demo *.o ch_*.out *.aux *.log *.zip

