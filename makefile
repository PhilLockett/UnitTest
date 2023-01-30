# Makefile for Logger unit tests.
objects  = test.o
objects += unittest.o

headers  = unittest.h

options = -std=c++20

test:	$(objects)	$(headers)
	g++ $(options) -o test $(objects)
	./test

%.o:	%.cpp	$(headers)
	g++ $(options) -c -o $@ $<

format:
	tfc -s -u -r test.cpp
	tfc -s -u -r unittest.cpp
	tfc -s -u -r unittest.h

clean:
	rm -f *.exe *.o
