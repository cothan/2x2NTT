CC = /usr/bin/c++
CFLAGS = -g3 -O0 -Wall -Wextra -Wpedantic #-Werror
RM = /bin/rm 

HEADERS = address_encoder_decoder.h butterfly_unit.h consts.h fifo.h ntt.h ntt2x2.h params.h ram_util.h reduce.h util.h 
SOURCES = address_encoder_decoder.cpp butterfly_unit.cpp fifo.cpp ntt.cpp consts.cpp ntt2x2.cpp ram_util.cpp reduce.cpp util.cpp 

.PHONY: all clean 

all: ntt2x2_test

ntt2x2_test: $(SOURCES) ntt2x2_test.cpp
	$(CC) $(SOURCES) $(CFLAGS) ntt2x2_test.cpp -o $@ 


