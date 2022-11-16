CC = g++
CFLAGS = -O3 -Wall
RM = /bin/rm 

HEADERS = fft.h   ../consts.h   params.h
SOURCES = fft.cpp ../consts.cpp 

.PHONY: all clean 

all: test_fft

test_fft: $(SOURCES) $(HEADERS) fft_test.cpp
	$(CC) $(SOURCES) $(CFLAGS) fft_test.cpp -o $@ 

clean:
	$(RM) test_fft

