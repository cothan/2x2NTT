#ifndef ADDRESS_ENCODER_DECODER_H
#define ADDRESS_ENCODER_DECODER_H

int resolve_address(enum MAPPING mapping, int addr);

void resolve_twiddle(unsigned tw_i[4], unsigned *last, 
                    unsigned tw_base_i[4],
                    const int k, const int s, enum OPERATION mode);

#endif
