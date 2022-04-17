exercise1: exercise1.o
	gcc exercise1.o -o exercise1

exercise1.o: exercise1.c
	gcc -c exercise1.c -o exercise1.o

exercise1.c:
	echo "My source code is to be stored here on Github"
	git clone "https://github.com/hoantrung112/network-programming.git"
	cp ./network-programming/hw-01/exercise1.c ./
	rm -rf ./network-programming
