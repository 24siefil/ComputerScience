CC = gcc

addr: addr.c backend-hash.o backend-hash.h
	${CC} -o addr addr.c backend-hash.o

backend-hash.o: backend-hash.c
	${CC} -c backend-hash.c

test: addr
	./addr < test1.in > test1.out
	diff ch_test1.out test1.out

clean:
	rm addr addr_demo *.o ch_*.out *.aux *.log *.zip
