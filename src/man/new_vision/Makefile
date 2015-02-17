CC = g++
CFLAGS= -g -Wall -std=c++0x

default: compile

compile: Blobber.o Blob.o
	$(CC) $? -o blobber

Blobber.o: Blobber.cpp
	$(CC) $(CFLAGS) -c $< -o $@

Blob.o: Blob.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o *~ blobber
