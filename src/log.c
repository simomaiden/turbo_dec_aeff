#include <math.h>
#include "log.h"
#include "param.h"
#include <stdio.h>

// bitwidth: ptr to bitwidth data structure
// default_channel: default channel LLR bitwidth
void compute_bitwidth(Bitwidth *bitwidth, Param *p) {
    int delta_g; 
    int delta_sm = 4 * pow(2, p->w);

    bitwidth->channel = p->w;
    bitwidth->extrinsic = p->w + 1;

    if (p->radix == 2) {
        delta_g = 2 * pow(2, p->w);

        bitwidth->branch = p->w + 2;
        bitwidth->state = (int) ceil(log2((double) 2*delta_sm + delta_g + 1) + 1);
    }   
    else if (p->radix == 4) {
        delta_g = 4 * pow(2, p->w);

        bitwidth->branch = p->w + 3;
        bitwidth->state = (int) ceil(log2( (double)  2*delta_sm + delta_g + 1) + 1);
    } 
    else if (p->radix == 8) {
        delta_g = 6 * pow(2, p->w);

        bitwidth->branch = p->w + 4;
        bitwidth->state = (int) ceil(log2( (double) 2*delta_sm + delta_g + 1) + 1);
    }
    else {
        delta_g = 8 * pow(2, p->w);

        bitwidth->branch = p->w + 5;
        bitwidth->state = (int) ceil(log2( (double) 2*delta_sm + delta_g + 1) + 1);
    }
}

// Estimated area increment factor from RCA to CLA on n-bits
double compute_CLA_area_increment_factor(int n) {
    return (double)((1.0/6*pow(n, 3) + pow(n, 2) + 35.0/6*n) / (9*n));
}

// Estimated clock frequency increment factor from RCA to CLA on n-bits.
double comput_CLA_frequency_increment_factor(int n) {
    return (3*n-1) / (4 + 2*log2(n+1));
}

double get_frequency(Bitwidth *bitwidth, Param *p) {
    double frequency;
    double f_incr_cla = 1;
    double cp_incr_rad4 = 1;
    double cp_incr_rad8 = 1;
    double cp_incr_rad16 = 1;
    double cp_scaling = (double) bitwidth->state / p->cp_ref_bits;

    if (p->real_cp) {
        if (p->pmu_model == 0) {
            cp_incr_rad4 = 1.36;
            cp_incr_rad8 = 2.31;
            cp_incr_rad16 = 2.31;
        }
        else if (p->pmu_model == 1) {
            cp_incr_rad4 = 1.36;
            cp_incr_rad8 = 1.81;
            cp_incr_rad16 = 1.81;
        }
    }
    else {
        cp_incr_rad4 = 1;
        cp_incr_rad8 = 1;
        cp_incr_rad16 = 1;
    }

    if (p->add == 1) {
        f_incr_cla = comput_CLA_frequency_increment_factor(bitwidth->state);
    }

    if (p->radix == 2) {
        frequency = (double)p->f/cp_scaling;
    }
    else if (p->radix == 4) {
        frequency = (double)p->f/cp_scaling/cp_incr_rad4;
    }
    else if (p->radix == 8) {
        frequency = (double)p->f/cp_scaling/cp_incr_rad8;
    }
    else {
        frequency = (double)p->f/cp_scaling/cp_incr_rad16;
    }

    frequency *= f_incr_cla;

    return frequency;
}

// bitwidth: ptr to bitwidth data structure
// p: ptr to model parameters data structure
double get_throughput(Bitwidth *bitwidth, Param *p) {
    double throughput;
    double frequency;

    frequency = get_frequency(bitwidth, p);

    if (p->radix == 2) {
        throughput = (double) p->K / (p->Kp+p->WS) * (int)frequency / p->n_hi;
    }
    else if (p->radix == 4) {
        throughput = (double) 2 * p->K / (p->Kp+p->WS) * frequency / p->n_hi;
    }
    else if (p->radix == 8) {
        throughput = (double) 3 * p->K / (p->Kp+p->WS) * frequency / p->n_hi;
    }
    else {
        throughput = (double) 4 * p->K / (p->Kp+p->WS) * frequency / p->n_hi;
    }

    return throughput;
}

double get_latency(Bitwidth *bitwidth, Param *p) {
    double latency;
    double frequency;

    frequency = get_frequency(bitwidth, p);

    if (p->radix == 2) {
        latency = (double) p->n_hi * (p->Kp+p->WS) * 1/frequency;
    }
    else if (p->radix == 4) {
        latency = (double) p->n_hi * (p->Kp+p->WS)/2 * 1/frequency;
    }
    else if (p->radix == 8) {
        latency = (double) p->n_hi * (p->Kp+p->WS)/3 * 1/frequency;
    }
    else {
        latency = (double) p->n_hi * (p->Kp+p->WS)/4 * 1/frequency;
    }

    return latency;
}

// p: ptr to model parameters data structure
double get_bmu_ge(Param *p) {
    double GE_value;

    if (p->radix == 2) {
        GE_value = 2 * p->GE_rca;
    }
    else if (p->radix == 4) {
        GE_value = 2 * (2*p->GE_rca) + 9*p->GE_rca;
    }
    else if (p->radix == 8) {
        GE_value = 3 * (2*p->GE_rca) + 54*p->GE_rca;
    }
    else {
        GE_value = 4 * (2*p->GE_rca) + 149*p->GE_rca + 64*p->GE_cs2;
    }

    return GE_value;
}

// p: ptr to model parameters data structure
double get_pmu_ge(Param *p) {
    double GE_value;
    
    // Max-Log-MAP
    if (p->algorithm == 0) {
        if (p->pmu_model == 0) {
            if (p->radix == 2) {
                GE_value = 8 * (2*p->GE_rca+1*p->GE_cs2) - 4*p->GE_rca;
            }
            else if (p->radix == 4) {
                GE_value = 8 * (4*p->GE_rca+1*p->GE_cs4_f) - 2*p->GE_rca;
            }
            else if (p->radix == 8) {
                GE_value = 8 * (8*p->GE_rca+4*p->GE_cs2+1*p->GE_cs4_f) - 1*p->GE_rca;
            }
            else {
                GE_value = 8 * (8*p->GE_rca+4*p->GE_cs2+1*p->GE_cs4_f) - 1*p->GE_rca;
            }
        }
        else if (p->pmu_model == 1) {
            if (p->radix == 2) {
                GE_value = 8 * (2*p->GE_rca+1*p->GE_cs2) - 4*p->GE_rca;
            }
            else if (p->radix == 4) {
                GE_value = 8 * (4*p->GE_rca+1*p->GE_cs4_f) - 2*p->GE_rca;
            }
            else if (p->radix == 8) {
                GE_value = 8 * (8*p->GE_rca+1*p->GE_cs8_f) - 1*p->GE_rca;
            }
            else {
                GE_value = 8 * (8*p->GE_rca+1*p->GE_cs8_f) - 1*p->GE_rca;
            }
        }
    }
    // Local-SOVA
    else if (p->algorithm == 1) {
        if (p->radix == 2) {
                GE_value = 16 * p->GE_rca + 8 * p->GE_cs2;
            }
            else if (p->radix == 4) {
                GE_value = 40 * p->GE_rca + 32 * p->GE_cs2;
            }
            else if (p->radix == 8) {
                GE_value = 96 * p->GE_rca + 88 * p->GE_cs2;
            }
            else {
                // TEMPORARY: Max-Log-MAP radix-16 IMPLEMENTATION (data not available)
                GE_value = 8 * (8*p->GE_rca+1*p->GE_cs8_f) - 1*p->GE_rca;
            }
    }
    return GE_value;
}

// p: ptr to model parameters data structure
double get_sou_ge(Param *p) {
    double GE_value;

    // Max-Log-MAP
    if (p->algorithm == 0) {
        if (p->radix == 2) {
            GE_value = 16*p->GE_rca + 14*p->GE_cs2 + 1*p->GE_rcs;
        }
        else if (p->radix == 4) {
            GE_value = 32*p->GE_rca + 4*(7*p->GE_cs2) + 4*p->GE_cs2 + 2*p->GE_rcs;
        }
        else if (p->radix == 8) {
            GE_value = 64*p->GE_rca + 8*(7*p->GE_cs2) + 6*(3*p->GE_cs2) + 3*p->GE_rcs;
        }
        else {
            GE_value = (128+64)*p->GE_rca + 16*(7*p->GE_cs2) + 8*(3*p->GE_cs2) + 8*p->GE_cs2 + 4*p->GE_rcs;
        }
    }
    // Local-SOVA
    else if (p->algorithm == 1) {
        if (p->radix == 2) {
            GE_value = 22 * p->GE_rca + 7 * p->GE_cs2;
        }
        else if (p->radix == 4) {
            GE_value = 22 * p->GE_rca + 21 * p->GE_cs2;
        }
        else if (p->radix == 8) {
            GE_value = 29 * p->GE_rca + 28 * p->GE_cs2;
        }
        else {
            // TEMPORARY: Max-Log-MAP IMPLEMENTATION
            GE_value = (128+64)*p->GE_rca + 16*(7*p->GE_cs2) + 8*(3*p->GE_cs2) + 8*p->GE_cs2 + 4*p->GE_rcs;
        }
    }

    return GE_value;
}

// bitwidth: ptr to bitwidth data structure
// p: ptr to model parameters data structureù
// log: ptr to logic area data structure (collecting informations if needed)
double get_siso_log_ge_area(Bitwidth *bitwidth, Param *p, logArea *log) {
    double siso_ge_area;
    double bmu_area_incr_cla = 1;
    double pmu_area_incr_cla = 1;
    double sou_area_incr_cla = 1;
    double bmu_scaling = (double) bitwidth->branch / p->log_ref_bits;
    double pmu_scaling = (double) bitwidth->state / p->log_ref_bits;
    double sou_scaling = pmu_scaling;

    if (p->add == 1) {
        bmu_area_incr_cla = compute_CLA_area_increment_factor(bitwidth->branch);
        pmu_area_incr_cla = compute_CLA_area_increment_factor(bitwidth->state);
        sou_area_incr_cla = compute_CLA_area_increment_factor(bitwidth->extrinsic);
    }

    double bmu_area = 2 * get_bmu_ge(p) * bmu_scaling * bmu_area_incr_cla;
    double pmu_area;
    double sou_area = get_sou_ge(p) * sou_scaling * sou_area_incr_cla;

    // MAX-Log-MAP
    if (p->algorithm == 0) {
        if (p->recomp > 0)
            pmu_area = 3 * get_pmu_ge(p) * pmu_scaling;
        else
            pmu_area = 2 * get_pmu_ge(p) * pmu_scaling;
    }
    // Local-SOVA
    else if (p->algorithm == 1) {
        // Just the backward PMU is different
        if (p->recomp > 0)
           pmu_area = 2 * get_pmu_ge(p) * pmu_scaling; 
        else
            pmu_area = get_pmu_ge(p) * pmu_scaling;
        p->algorithm = 0;
        pmu_area += get_pmu_ge(p) * pmu_scaling;
        p->algorithm = 1;
    }

    pmu_area *= pmu_area_incr_cla;

    siso_ge_area = bmu_area + pmu_area + sou_area;

    if (log != NULL) {
        log->bmu = bmu_area * p->ge_area / 1e6;
        log->pmu = pmu_area * p->ge_area / 1e6;
        log->sou = sou_area * p->ge_area / 1e6;
    } 
    return siso_ge_area;
}

// bitwidth: ptr to bitwidth data structure
// p: ptr to model parameters data structure
// log: ptr to logic area data structure (collecting informations if needed)
double get_turbo_log_ge_area(Bitwidth *bitwidth, Param *p, logArea *log) {
    double total_area = (double) p->K / p->Kp * get_siso_log_ge_area(bitwidth, p, NULL) * p->ge_area / 1e6;

    if (log != NULL) {
        get_siso_log_ge_area(bitwidth, p, log);
        log->bmu *= p->K / p->Kp;
        log->pmu *= p->K / p->Kp;
        log->sou *= p->K / p->Kp;
        log->total = total_area;
    }
    return total_area;
}