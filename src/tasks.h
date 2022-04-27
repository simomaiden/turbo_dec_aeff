#ifndef _TASKS_H
#define _TASKS_H

#include "param.h"

void compute_radix_bitwidths(Bitwidth bitwidths[], Param *p);
void single_radix_analysis(ArchData arch[], Bitwidth bitwidths[], Param *p);
int get_total_architectures(ExplorationParam *expl_p);
void multiple_radix_analysis(ArchData arch[], Bitwidth bitwidths[], ExplorationParam *expl_p, Param *p);
void reliability_radix_analysis(ReliabilityParam *rel_param, ReliabilityResult *rel_res, Bitwidth bitwidths[], Param *p);

void avg_increment_factors(Bitwidth bitwidths[], ExplorationParam *expl_p, Param *p);

#endif