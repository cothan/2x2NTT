CC = /usr/bin/c++
CFLAGS = -O3 -Wall
RM = /bin/rm 

HEADERS = ref_ntt.h   ref_ntt2x2.h   ../consts.h ../params.h
SOURCES = ref_ntt.cpp ref_ntt2x2.cpp ../consts.cpp

.PHONY: all clean 

all: ref_test_ntt_ntt2x2

ref_test_ntt_ntt2x2: $(SOURCES) $(HEADERS) ref_test_ntt_ntt2x2.cpp
	$(CC) $(SOURCES) $(CFLAGS) ref_test_ntt_ntt2x2.cpp -o $@ 

clean:
	$(RM) ref_test_ntt_ntt2x2

