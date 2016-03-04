#Makefile

smartpointer:	sharedptr.o
	g++ -std=c++11 -g -Wall -Wextra -pedantic -pthread test.cpp -o smartpointer

sharedptr.o:
	g++ -std=c++11 -c SharedPtr.hpp -o sharedptr.o

clean:
	rm -f *.o *~ smartpointer
