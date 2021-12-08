#ifndef _RESULT_H
#define _RESULT_H

#include "param.h"
#include <stdio.h>

void save_single_radix_analysis(ArchData arch[], char filename[], char open_option[]);
void save_dataset(ArchData arch[], int arch_num, char filename[], char open_option[]);
void save_reliability_analysis(ReliabilityResult *rel_res, char filename[], char open_option[]);

#endif