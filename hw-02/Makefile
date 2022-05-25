all: resolver

resolver: resolver.o
	gcc resolver.o -o resolver
	   
resolver.o: resolver.c
	gcc -c resolver.c -o resolver.o
	
clean: 	     
	rm -rf *o resolver
