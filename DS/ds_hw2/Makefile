CC = gcc

addr2: addr2.c backend2.o backend2.h
	${CC} -o addr2 addr2.c backend2.o

backend2.o: backend2.c backend2.h
	${CC} -c backend2.c

test: addr2 test1.in
	./addr2 < test1.in > test1.out
	diff ch_test1.out test1.out

clean:
	rm addr2 addr2_demo *.o *.out *.aux *.log *.zip
