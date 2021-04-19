CC = /usr/bin/clang++

HEADERS = address_encoder_decoder.h   butterfly_unit.h   consts.h   fifo.h   ntt2x2.h   ram_util.h   reduce.h   mytypes.h   params.h
SOURCES = address_encoder_decoder.cpp butterfly_unit.cpp consts.cpp fifo.cpp ntt2x2.cpp ram_util.cpp reduce.cpp 

.PHONY: all clean 

all: ntt2x2_test

ntt2x2_test: $(SOURCES) ntt2x2_test.cpp
	$(CC) $(SOURCES) $(CFLAGS) ntt2x2_test.cpp -o $@ 

