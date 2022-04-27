#include <stdio.h>
#include <malloc.h>
#include "param.h"
#include "log.h"
#include "mem.h"
#include "tasks.h"
#include "result.h"

int main(int argc, char *argv[]) {

    // Check on the argument
    if (argc != 2) {
        printf("Input file not found.\n");
        return -1;
    }

    // Initial data structures
    Param p;                            // Generic parameters
    ExplorationParam expl_p;            // Design exploration parameters
    ReliabilityParam rel_p;             // Reliability parameters

    // Data structures initialization from input file
    initialize_parameters(argv[1], &p, &expl_p, &rel_p);

    ArchData single_arch[4];            // Single analysis architectures
    Bitwidth bitwidths[4];              // Bitwidths data structures
    ReliabilityResult rel_res;          // Reliability results data structure

    // Bitwidths computation for different radix-orders
    compute_radix_bitwidths(bitwidths, &p);

    // Total number of analyzed architecture for the multiple analysis
    int total_architectures = get_total_architectures(&expl_p);
    // Multiple analysis architectures
    ArchData *multiple_arch = (ArchData*) malloc(total_architectures * sizeof(ArchData));

    // Single analysis
    single_radix_analysis(single_arch, bitwidths, &p);
    // Reliability analysis on the radix-order selection
    reliability_radix_analysis(&rel_p, &rel_res, bitwidths, &p);
    // Multiple analysis exploring the architectural space
    multiple_radix_analysis(multiple_arch, bitwidths, &expl_p, &p);

    // Saving results
    save_single_radix_analysis(single_arch, p.s_filename, "w");
    save_reliability_analysis(&rel_res, p.s_filename, "a");
    save_dataset(multiple_arch, total_architectures, p.m_filename, "w");

    // Custom data analysis
    //avg_increment_factors(bitwidths, &expl_p, &p);
    
    return 0;
}