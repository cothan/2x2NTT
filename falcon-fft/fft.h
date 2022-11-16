#ifndef FFT_H
#define FFT_H

#include "params.h"

void FFT(fpr *f, unsigned logn);
void iFFT(fpr *f, unsigned logn);

void fwd_FFT_short(fpr *f, unsigned logn);
void inv_FFT_short(fpr *f, unsigned logn);

void fwd_FFT_adj_short(fpr *f, unsigned logn);
void inv_FFT_adj_short(fpr *f, unsigned logn);

void fwd_FFT_adj(fpr *f, unsigned logn);
void inv_FFT_adj(fpr *f, unsigned logn);

#endif