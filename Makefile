test:   nestedpipe.o 
	cc -o test nestedpipe.o 

nestedpipe.o: nestedpipe.c
	cc -c nestedpipe.c

clean:
	rm nestedpipe.o test
