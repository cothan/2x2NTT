CC = /usr/bin/cc
CFLAGS = -g3 -O0 -Wall -Wextra -Wpedantic #-Werror
RM = /bin/rm 

HEADERS = address_encoder_decoder.h butterfly_unit.h consts.h fifo.h ntt.h ntt2x2.h params.h ram_util.h reduce.h util.h 
SOURCES = address_encoder_decoder.c butterfly_unit.c consts.c fifo.c ntt.c ntt2x2.c ram_util.c reduce.c util.c 

.PHONY: all clean 

all: ntt2x2_test

ntt2x2_test: $(SOURCES) ntt2x2_test.c
	$(CC) $(SOURCES) $(CFLAGS) ntt2x2_test.c -o $@ 


