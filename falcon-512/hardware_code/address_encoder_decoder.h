#ifndef ADDRESS_ENCODER_DECODER_H
#define ADDRESS_ENCODER_DECODER_H

unsigned resolve_address(enum MAPPING mapping, unsigned addr);

void resolve_twiddle(unsigned tw_i[4], unsigned *last, 
                    unsigned tw_base_i[4],
                    const int k, const int s, enum OPERATION mode);

#endif
