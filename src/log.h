#ifndef _LOG_H
#define _LOG_H

#include "param.h"

void compute_bitwidth(Bitwidth *bitwidth, Param *p);
double get_frequency(Bitwidth *bitwidth, Param *p);
double get_throughput(Bitwidth *bitwidth, Param *p);
double get_latency(Bitwidth *bitwidth, Param *p);
double get_pmu_ge(Param *p);
double get_bmu_ge(Param *p);
double get_sou_ge(Param *p);
double get_siso_log_ge_area(Bitwidth *bitwidth, Param *p, logArea *log);
double get_turbo_log_ge_area(Bitwidth *bitwidth, Param *p, logArea *log);

#endif