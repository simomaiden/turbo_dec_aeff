#ifndef _MEM_H
#define _MEM_H

#include "param.h"

double get_ge_bit(int words, Param *p);
int get_in_frame_words(Param *p);
int get_extr_inf_words(Param *p);
int get_alpha_words(Param *p);
int get_nii_words(Param *p);
double get_crossbar_ge_area(int banks_num, Bitwidth *bitwidth, Param *p);
double get_turbo_mem_ge_area(Bitwidth *bitwidth, Param *p);

#endif