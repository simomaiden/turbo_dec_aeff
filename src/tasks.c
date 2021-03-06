#include <malloc.h>
#include <math.h>
#include "tasks.h"
#include "param.h"
#include "log.h"
#include "mem.h"
#include <stdio.h>

// bitwidths: array of [Bitwidth] to be filled
// p: ptr to model parameters data structure
void compute_radix_bitwidths(Bitwidth bitwidths[], Param *p) {
    // Filling the bitwidths structures
    p->radix = 2;
    compute_bitwidth(&bitwidths[0], p);
    p->radix = 4;
    compute_bitwidth(&bitwidths[1], p);
    p->radix = 8;
    compute_bitwidth(&bitwidths[2], p);
    p->radix = 16;
    compute_bitwidth(&bitwidths[3], p);
}

// arch: array of [ArchData] to be filled with computed results
// bitwidths: array of [Bitwidth] struct to be filled
// p: ptr to model parameters data structure
void single_radix_analysis(ArchData arch[], Bitwidth bitwidths[], Param *p) {
    int radix_orders[4] = {2, 4, 8, 16};
    double frequency;
    double th;
    double lat;
    double area;
    double aeff;
    double power;
    double power_density;
    double energy_eff;
    int parallel_decoders;
    int i;

    // Analyzing the radix orders
    for (i = 0; i < 4; i++) {
        p->radix = radix_orders[i];

        frequency = get_frequency(&bitwidths[i], p);
        th = get_throughput(&bitwidths[i], p);
        lat = get_latency(&bitwidths[i], p);
        get_turbo_log_ge_area(&bitwidths[i], p, &arch[i].log_area);
        get_turbo_mem_ge_area(&bitwidths[i], p, &arch[i].mem_area);
        area = arch[i].log_area.total + arch[i].mem_area.total;
        aeff = th / area;
        parallel_decoders = (int) floor(p->av_area / (area));
        power = p->pow_per_f_area * frequency * area / 1e6;
        power_density = power / area;
        energy_eff = power / th / area;

        // Saving the results in the architecture data structure
        arch[i].K = p->K;
        arch[i].Kp = p->Kp;
        arch[i].WS = p->WS;
        arch[i].w = p->w;
        arch[i].radix = radix_orders[i];
        arch[i].throughput = th/1e9;                                            // [Gb/s]
        arch[i].total_area = area;                                              // [mm^2]
        arch[i].latency = lat*1e6;                                              // [us]
        arch[i].aeff = aeff/1e9;                                                // [Gb/s/mm^2]
        arch[i].throughput_on_area = (double) parallel_decoders * th/1e9;       // [Gb/s]
        arch[i].power = power;                                                  // [W]
        arch[i].power_density = power_density;                                  // [W/mm^2]
        arch[i].energy_eff = energy_eff * 1e12;                                 // [pJ/bit/mm^2]
    }
}

// start: first point in the array
// last: last point in the array
// step: step between two points in the array
int* get_param_array(int start, int stop, int step) {
    static int *array;
    int range = stop - start;
    int size = (int) range/step + 1;
    int i;

    array = (int*) malloc(size * sizeof(int));

    // Array initialization
    for (i = 0; i < size; i++) {
        array[i] = start + i*step;
    }
        
    return array;    
}

// start: first point in the array
// last: last point in the array
// step: step between two points in the array
int get_param_array_size(int start, int stop, int step) {
    int range = stop - start;
    int size = (int) range/step + 1;

    return size;
}

// best_arch: [ArchData] struct to store the best radix-order results
// bitwidths: array of [Bitwidth] struct to be filled
// p: ptr to model parameters data structure
// SOME DATA TO BE COMPUTED ARE MISSING
void best_radix_analysis(ArchData *best_arch, Bitwidth bitwidths[], Param *p) {
    int radix_orders[4] = {2, 4, 8, 16};
    double th;
    double lat;
    double area;
    int parallel_decoders;
    double aeff;
    double best_aeff = -1;
    double frequency;
    double power;
    double power_density;
    int i;

    // Analyzing the radix orders
    for (i = 0; i < 4; i++) {
        p->radix = radix_orders[i];

        // Parameters computation
        frequency = get_frequency(&bitwidths[i], p);
        th = get_throughput(&bitwidths[i], p);
        lat = get_latency(&bitwidths[i], p);
        get_turbo_log_ge_area(&bitwidths[i], p, &best_arch[i].log_area);
        get_turbo_mem_ge_area(&bitwidths[i], p, &best_arch[i].mem_area);
        area = best_arch[i].log_area.total + best_arch[i].mem_area.total;
        aeff = th / (area);
        parallel_decoders = (int) floor(p->av_area / (area));
        power = p->pow_per_f_area * frequency * area / 1e6;
        power_density = power / area;

        // Saving the results in the architecture data structure
        if (aeff > best_aeff) {
            best_aeff = aeff;

            best_arch->K = p->K;
            best_arch->Kp = p->Kp;
            best_arch->WS = p->WS;
            best_arch->w = p->w;
            best_arch->radix = radix_orders[i];
            best_arch->throughput = th/1e9;                                         // [Gb/s]
            best_arch->power = power;                                               // [W]
            best_arch->power_density = power_density;                               // [W/mm^2]
            best_arch->total_area = (area);                                       // [mm^2]
            best_arch->latency = lat*1e6;                                         // [us]
            best_arch->aeff = aeff/1e9;                                             // [Gb/s/mm^2]
            best_arch->throughput_on_area = (double) parallel_decoders * th/1e9;    // [Gb/s]
            best_arch->pmap_num = parallel_decoders;
        }
    }
}

// expl_p: ptr to exploration parameters data structure
int get_total_architectures(ExplorationParam *expl_p) {
    // Get arrays sizes
    int N_K = get_param_array_size(expl_p->K[0], expl_p->K[1], expl_p->K[2]);
    int N_Kp = get_param_array_size(expl_p->Kp[0], expl_p->Kp[1], expl_p->Kp[2]);
    int N_WS = get_param_array_size(expl_p->WS[0], expl_p->WS[1], expl_p->WS[2]);
    int N_w = get_param_array_size(expl_p->w[0], expl_p->w[1], expl_p->w[2]);

    return N_K * N_Kp * N_WS * N_w;
}

// arch: array of [ArchData] to be filled with computed results
// expl_p: ptr to exploration parameters data structure
// p: ptr to model parameters data structure
void multiple_radix_analysis(ArchData arch[], Bitwidth bitwidths[], ExplorationParam *expl_p, Param *p) {
    // Get arrays sizes
    int N_K = get_param_array_size(expl_p->K[0], expl_p->K[1], expl_p->K[2]);
    int N_Kp = get_param_array_size(expl_p->Kp[0], expl_p->Kp[1], expl_p->Kp[2]);
    int N_WS = get_param_array_size(expl_p->WS[0], expl_p->WS[1], expl_p->WS[2]);
    int N_w = get_param_array_size(expl_p->w[0], expl_p->w[1], expl_p->w[2]);
    
    // Get arrays
    int *K = get_param_array(expl_p->K[0], expl_p->K[1], expl_p->K[2]);
    int *Kp = get_param_array(expl_p->Kp[0], expl_p->Kp[1], expl_p->Kp[2]);
    int *WS = get_param_array(expl_p->WS[0], expl_p->WS[1], expl_p->WS[2]);
    int *w = get_param_array(expl_p->w[0], expl_p->w[1], expl_p->w[2]);

    int i, j, k, l;
    int arch_index = 0;

    // K loop
    for (i = 0; i < N_K; i++) {
        // Kp loop
        for (j = 0; j < N_Kp; j++) {
            // WS loop
            for (k = 0; k < N_WS; k++) {
                // w loop
                for (l = 0; l < N_w; l++) {
                    p->K = K[i];
                    p->Kp = Kp[j];
                    p->WS = WS[k];
                    p->w = w[l];

                    best_radix_analysis(&arch[arch_index], bitwidths, p);
                    arch_index++;
                }
            }
        }
    }
}

// rel_param: ptr to the realiability parameters structure
// rel_res: ptr to the reliability results structure
// bitwidths: array of [Bitwidth] struct to be filled
// p: ptr to model parameters data structure
void reliability_radix_analysis(ReliabilityParam *rel_param, ReliabilityResult *rel_res, Bitwidth bitwidths[], Param *p) {
    int radix_orders[4] = {2, 4, 8, 16};
    double th;
    double log_area;
    double mem_area;
    double aeff;
    double best_aeff = -1;
    int best_radix;
    int radix2_solutions = 0;
    int radix4_solutions = 0;
    int radix8_solutions = 0;
    int radix16_solutions = 0;
    int total_tests;            // Total number of performed tests
    double log_err_factor;      // Logic error multiplication factor
    double mem_err_factor;      // Memory error multiplication factor
    double th_err_factor;       // Throughput error multiplication factor
    int i, j, k, l;

    // Get error arrays size
    int log_err_size = get_param_array_size(-rel_param->log_err, rel_param->log_err, 1);
    int mem_err_size = get_param_array_size(-rel_param->mem_err, rel_param->mem_err, 1);
    int th_err_size = get_param_array_size(-rel_param->th_err, rel_param->th_err, 1);
    total_tests = log_err_size * mem_err_size * th_err_size;

    // Get error arrays
    int *log_err = get_param_array(-rel_param->log_err, rel_param->log_err, 1);
    int *mem_err = get_param_array(-rel_param->mem_err, rel_param->mem_err, 1);
    int *th_err = get_param_array(-rel_param->th_err, rel_param->th_err, 1);

    // Logic error loop
    for (i = 0; i < log_err_size; i++) {
        // Memory error loop
        for (j = 0; j < mem_err_size; j++) {
            // Throughput error loop
            for (k = 0; k < th_err_size; k++) {
                // Calculating error factors
                log_err_factor = 1 + (double)log_err[i]/100;
                mem_err_factor = 1 + (double)mem_err[j]/100;
                th_err_factor = 1 + (double)th_err[k]/100;

                // Initializing the best area efficiency
                best_aeff = -1;

                // Analyzing the radix orders
                for (l = 0; l < 4; l++) {
                    p->radix = radix_orders[l];

                    // Parameters computation
                    if (radix_orders[l] != 2) {
                        th = get_throughput(&bitwidths[l], p) * th_err_factor;
                    }
                    else {
                        th = get_throughput(&bitwidths[l], p);
                    }
                    log_area = get_turbo_log_ge_area(&bitwidths[l], p, NULL) * log_err_factor;
                    mem_area = get_turbo_mem_ge_area(&bitwidths[l], p, NULL) * mem_err_factor;
                    aeff = th / (log_area + mem_area);

                    // Saving the results in the architecture data structure
                    if (aeff > best_aeff) {
                        best_aeff = aeff;
                        best_radix = radix_orders[l];
                    }
                }

                // Incrementing the counters
                if (best_radix == 2) {
                    radix2_solutions++;
                }
                else if (best_radix == 4) {
                    radix4_solutions++;
                }
                else if (best_radix == 8) {
                    radix8_solutions++;
                }
                else {
                    radix16_solutions++;
                }
            }
        }
    }

    rel_res->rad2_perc = 100 * (double) radix2_solutions/total_tests;
    rel_res->rad4_perc = 100 * (double) radix4_solutions/total_tests;
    rel_res->rad8_perc = 100 * (double) radix8_solutions/total_tests;
    rel_res->rad16_perc = 100 * (double) radix16_solutions/total_tests;
}

/////////////////////////////////////
// CUSTOM DATA ANALSYSIS FUNCTIONS //
/////////////////////////////////////

// bitwidths: array of [Bitwidth] struct to be filled
// expl_p: ptr to exploration parameters data structure
// p: ptr to model parameters data structure
void avg_increment_factors(Bitwidth bitwidths[], ExplorationParam *expl_p, Param *p) {
    int radix_orders[4] = {2, 4, 8, 16};
    double pow_incr_fact;
    double min_aeff_incr[4];
    double max_aeff_incr[4];
    double avg_th_incr[4];
    double avg_ar_incr[4];
    double avg_pow_incr[4];

    // Get arrays sizes
    int N_K = get_param_array_size(expl_p->K[0], expl_p->K[1], expl_p->K[2]);
    int N_Kp = get_param_array_size(expl_p->Kp[0], expl_p->Kp[1], expl_p->Kp[2]);
    int N_WS = get_param_array_size(expl_p->WS[0], expl_p->WS[1], expl_p->WS[2]);
    int N_w = get_param_array_size(expl_p->w[0], expl_p->w[1], expl_p->w[2]);
    
    // Get arrays
    int *K = get_param_array(expl_p->K[0], expl_p->K[1], expl_p->K[2]);
    int *Kp = get_param_array(expl_p->Kp[0], expl_p->Kp[1], expl_p->Kp[2]);
    int *WS = get_param_array(expl_p->WS[0], expl_p->WS[1], expl_p->WS[2]);
    int *w = get_param_array(expl_p->w[0], expl_p->w[1], expl_p->w[2]);

    int i, j, k, l, m;
    double th, th_rad2;
    double log_area;
    double mem_area;
    double aeff, aeff_rad2;
    double area, area_rad2;

    for (i = 0; i < 4; i++) {
        min_aeff_incr[i] = 1000;
        max_aeff_incr[i] = 0;
    }

    // K loop
    for (i = 0; i < N_K; i++) {
        // Kp loop
        for (j = 0; j < N_Kp; j++) {
            // WS loop
            for (k = 0; k < N_WS; k++) {
                // w loop
                for (l = 0; l < N_w; l++) {
                    p->K = K[i];
                    p->Kp = Kp[j];
                    p->WS = WS[k];
                    p->w = w[l];

                    for (m = 0; m < 4; m++) {
                        p->radix = radix_orders[m];

                        // Parameters computation
                        th = get_throughput(&bitwidths[m], p);
                        log_area = get_turbo_log_ge_area(&bitwidths[m], p, NULL);
                        mem_area = get_turbo_mem_ge_area(&bitwidths[m], p, NULL);
                        area = log_area + mem_area;
                        aeff = th / area;

                        if (p->radix == 2) {
                            th_rad2 = th;
                            area_rad2 = area;
                            aeff_rad2 = aeff;
                        }

                        // Power increment factor = (freq. increment * area increment)
                        pow_incr_fact = th/th_rad2/(m+1) * area/area_rad2;

                        avg_th_incr[m] += th/th_rad2;
                        avg_ar_incr[m] += area/area_rad2;
                        avg_pow_incr[m] += pow_incr_fact;

                        if (aeff/aeff_rad2 > max_aeff_incr[m])
                            max_aeff_incr[m] = aeff/aeff_rad2;
                        if (aeff/aeff_rad2 < min_aeff_incr[m])
                            min_aeff_incr[m] = aeff/aeff_rad2;
                    }
                }
            }
        }
    }

    for (i = 0; i < 4; i++) {
        avg_th_incr[i] = avg_th_incr[i] / (N_K*N_Kp*N_WS*N_w);
        avg_ar_incr[i] = avg_ar_incr[i] / (N_K*N_Kp*N_WS*N_w);
        avg_pow_incr[i] = avg_pow_incr[i] / (N_K*N_Kp*N_WS*N_w);
    }

    printf("\tRadix 2 Increment Factors\n");
    printf("Radix\tArea\tTh.\tPower\tMax. Aeff\tMin. Aeff\n");
    printf("2\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t\t%.2lf\n", avg_ar_incr[0], avg_th_incr[0], avg_pow_incr[0], max_aeff_incr[0], min_aeff_incr[0]);
    printf("4\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t\t%.2lf\n", avg_ar_incr[1], avg_th_incr[1], avg_pow_incr[1], max_aeff_incr[1], min_aeff_incr[1]);
    printf("8\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t\t%.2lf\n", avg_ar_incr[2], avg_th_incr[2], avg_pow_incr[2], max_aeff_incr[2], min_aeff_incr[2]);
    printf("16\t%.2lf\t%.2lf\t%.2lf\t%.2lf\t\t%.2lf\n", avg_ar_incr[3], avg_th_incr[3], avg_pow_incr[3], max_aeff_incr[3], min_aeff_incr[3]);
    printf("\n\n");
}