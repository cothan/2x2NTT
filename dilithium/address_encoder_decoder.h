#ifndef ADDRESS_ENCODER_DECODER_H
#define ADDRESS_ENCODER_DECODER_H

int addr_decoder(int addr_in);

int addr_encoder(int addr_in);

int resolve_address(enum MAPPING mapping, int addr);

#endif