CC = gcc

addr1: addr1.c backend1.o backend1.h
	${CC} -o addr1 addr1.c backend1.o

backend1.o: backend1.c backend1.h
	${CC} -c backend1.c

test: addr1 test1.in
	./addr1 < test1.in > test1.out
	diff ch_test1.out test1.out

clean:
	rm addr1 addr1_demo *.o *.out *.aux *.log *.zip
