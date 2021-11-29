CC = /usr/bin/c++
CFLAGS = -O3 -Wall
RM = /bin/rm 

REF_DIR = ../reference_code

REF_HEADERS = ../params.h
REF_HEADERS += ../consts.h   $(REF_DIR)/ref_ntt.h   $(REF_DIR)/ref_ntt2x2.h
REF_SOURCES  = ../consts.cpp $(REF_DIR)/ref_ntt.cpp $(REF_DIR)/ref_ntt2x2.cpp

HEADERS = address_encoder_decoder.h config.h ntt2x2.h util.h butterfly_unit.h fifo.h ram_util.h
SOURCES = address_encoder_decoder.cpp util.cpp ram_util.cpp ntt2x2.cpp ntt2x2_fwdntt.cpp

.PHONY: all clean 

all: ntt2x2_test

ntt2x2_test: $(SOURCES) $(HEADERS) $(REF_HEADERS) $(REF_SOURCES) ntt2x2_test.cpp
	$(CC)  -o $@  $(REF_SOURCES) $(SOURCES) ntt2x2_test.cpp $(CFLAGS) 

clean:
	$(RM) ntt2x2_test

