# Makefile for Logger unit tests.
objects  = test.o
objects += unittest.o

headers  = unittest.h

options = -std=c++20

test:	$(objects)	$(headers)
	g++ $(options) -o test $(objects)

%.o:	%.cpp	$(headers)
	g++ $(options) -c -o $@ $<

clean:
	rm -f *.exe *.o
