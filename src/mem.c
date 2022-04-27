#include <stdlib.h>
#include "param.h"
#include "mem.h"
#include <math.h>

// words: required number of words for the memory
// p: ptr to model parameters data structure
double get_ge_bit(int words, Param *p) {
    double ge;
    // Constants from the curve fit
    double k1 = 0.0019762813983789074;
    double k2 = 2.1131083781620728;
    double k3 = 1.1481930744119049;

    // Equation to model the GE/bit variation for library memories
    ge = p->ge_cell + (double) 1 / (k2*log(k3+fabs((double)k1 * words)));

    // Check for synthesized memories
    if (words > p->min_words-1) {
        return ge;
    }
    else {
        return p->ge_synth_mem;
    }
}

// p: ptr to model parameters data structure
int get_in_frame_words(Param *p) {
    return p->K;
}

// p: ptr to model parameters data structure
int get_extr_inf_words(Param *p) {
    return p->K;
}

// p: ptr to model parameters data structure
int get_alpha_words(Param *p) {
    int alpha_words;
    int n_siso = (int) ceil((double)p->K / p->Kp);

    if (p->radix == 2) {
        alpha_words = n_siso * p->WS;
    }
    else if (p->radix == 4) {
        alpha_words = (int) (double)n_siso * (double)p->WS/2;
    }
    else if (p->radix == 8) {
        alpha_words = (int) (double)n_siso * (double)p->WS/3;
    }
    else {
        alpha_words = (int) (double)n_siso * (double)p->WS/4;
    }
    return alpha_words;
}

// p: ptr to model parameters data structure
int get_nii_words(Param *p) {
    int n_siso = (int) ceil((double) p->K / p->Kp);
    return (int) ceil((double)n_siso * (double)p->Kp/p->WS);
}

// banks_num: number of memory banks required to be managed
// bitwidth: ptr to bitwidth data structure
// p: ptr to model parameters data structure
double get_crossbar_ge_area(int banks_num, Bitwidth *bitwidth, Param *p) {
    return ((double)bitwidth->extrinsic/p->crbar_ref_bits) * p->GE_crbar * pow(((double)banks_num/p->banks_ref), 2);
}

double get_turbo_mem_ge_area(Bitwidth *bitwidth, Param *p, MemArea *mem) {
    double ge_area;
    int n_siso = (int) ceil((double) p->K / p->Kp);

    // Inititial required partitions
    int in_frame_p = n_siso;
    int extr_p = n_siso;
    int alpha_p = n_siso;
    int nii_p = n_siso;

    // Number of words per memory partition (permutation memory)
    int perm_words_p = p->Kp+p->WS;  // It is latency related

    // Effect of the Forward-Backward Scheduling on local memories
    alpha_p *= 2;

    // Effect of the radix and the Forward-Backward scheduling on global memories
    if (p->radix == 2) {
        in_frame_p *= 2;
        extr_p *= 3;
    }
    else if (p->radix == 4) {
        in_frame_p *= 4;
        extr_p *= 6;
        perm_words_p /= 2;
    }
    else if (p->radix == 8) {
        in_frame_p *= 6;
        extr_p *= 9;
        perm_words_p /= 3;
    }
    else {
        in_frame_p *= 8;
        extr_p *= 12;
        perm_words_p /= 4;
    }

    int perm_p = extr_p;
    int perm_bitwidth = (int) floor(log2(extr_p)+1);

    // Number of words per memory partition
    int in_frame_words_p = (int) ceil( (double) get_in_frame_words(p) / in_frame_p);
    int extr_words_p = (int) ceil( (double) get_extr_inf_words(p) / extr_p);
    int alpha_words_p = (int) ceil( (double) get_alpha_words(p) / alpha_p);
    int nii_words_p = (int) ceil( (double) get_nii_words(p) / nii_p);

    // GE estimation
    double ge_in_frame = get_ge_bit(in_frame_words_p, p);
    double ge_extr_inf = get_ge_bit(extr_words_p, p);
    double ge_alpha = get_ge_bit(alpha_words_p, p);
    double ge_nii = get_ge_bit(nii_words_p, p);
    double ge_perm = get_ge_bit(perm_words_p, p);

    // Area computation: words/partition * partitions * word bitwidth * GE/bit
    double ge_in_frame_area = (in_frame_words_p*in_frame_p) * 3*bitwidth->channel * ge_in_frame;
    double ge_extr_inf_area = (extr_words_p*extr_p) * bitwidth->extrinsic * ge_extr_inf;
    double ge_alpha_area = (alpha_words_p*alpha_p) * 8*bitwidth->state * ge_alpha;
    double ge_nii_area = (nii_words_p*nii_p) * 8*bitwidth->state * ge_nii;
    double ge_perm_area = (perm_words_p*perm_p) * perm_bitwidth * ge_perm;

    // Recomputation alpha memory reduction
    ge_alpha_area *= (double)((100.0 - p->recomp)/100);

    // Crossbar area estimation
    double ge_crossbar = get_crossbar_ge_area(extr_p, bitwidth, p);

    // Total area estimation
    ge_area = ge_in_frame_area + ge_extr_inf_area + ge_alpha_area + ge_nii_area + ge_crossbar + ge_perm_area;

    if (mem != NULL) {
        mem->in_frame = ge_in_frame_area * p->ge_area / 1e6;
        mem->alpha = ge_alpha_area * p->ge_area / 1e6;
        mem->extrinsic = ge_extr_inf_area * p->ge_area / 1e6;
        mem->nii = ge_nii_area * p->ge_area / 1e6;
        mem->crossbar = ge_crossbar * p->ge_area / 1e6;
        mem->perm = ge_perm_area * p->ge_area / 1e6;
        mem->total = ge_area * p->ge_area / 1e6;
    }

    return ge_area * p->ge_area / 1e6;
}

