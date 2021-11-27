#ifndef ADDRESS_ENCODER_DECODER_H
#define ADDRESS_ENCODER_DECODER_H

int resolve_address(enum MAPPING mapping, int addr);

void resolve_twiddle(uint16_t tw_i[4], uint16_t *last, uint16_t tw_base_i[4],
                    const int k, const int s, enum OPERATION mode);

#endif
