CC = /usr/bin/cc 
CFLAGS = -O3 -Wall
RM = /bin/rm 

HEADERS = ref_ntt.h ref_ntt2x2.h ../consts.h ../params.h
SOURCES = ref_ntt.c ref_ntt2x2.c ../consts.cpp

.PHONY: all clean 

all: ref_test_ntt_ntt2x2

ref_test_ntt_ntt2x2: $(SOURCES) $(HEADERS) ref_test_ntt_ntt2x2.c
	$(CC) $(SOURCES) $(CFLAGS) ref_test_ntt_ntt2x2.c -o $@ 

clean:
	$(RM) ref_test_ntt_ntt2x2

