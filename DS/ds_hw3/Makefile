CC = gcc

addr: addr.o backend.o memory.o
	${CC} -o addr addr.o backend.o memory.o

addr.o: addr.c backend.h
	${CC} -c addr.c

backend.o: backend.c backend.h memory.h memory.o # instead of memory.c
	${CC} -c backend.c

memory.o: # memory.c memory.h
	${CC} -c memory.c

test: addr
	./addr < test1.in > test1.out
	diff ch_test1.out test1.out

clean:
	rm addr addr_demo *.o ch_*.out *.log *.zip
