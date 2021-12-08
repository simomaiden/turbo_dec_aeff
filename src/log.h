#ifndef _LOG_H
#define _LOG_H

#include "param.h"

void compute_bitwidth(Bitwidth *bitwidth, Param *p);
double get_throughput(Bitwidth *bitwidth, Param *p);
double get_pmu_ge(Param *p);
double get_bmu_ge(Param *p);
double get_sou_ge(Param *p);
double get_siso_log_ge_area(Bitwidth *bitwidth, Param *p);
double get_turbo_log_ge_area(Bitwidth *bitwidth, Param *p);

#endif