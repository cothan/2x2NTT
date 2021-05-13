#ifndef ADDRESS_ENCODER_DECODER_H
#define ADDRESS_ENCODER_DECODER_H

int addr_decoder(int addr_in);

int addr_encoder(int addr_in);

int resolve_address(enum MAPPING mapping, int addr);

void resolve_twiddle(int32_t tw_i[4], int *last, int32_t tw_base_i[4],
                    const int k, const int s, enum OPERATION mode);

#endif