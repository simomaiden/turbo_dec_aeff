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

// bitwidth: ptr to bitwidth data structure
// p: ptr to model parameters data structure
double get_throughput(Bitwidth *bitwidth, Param *p) {
    double throughput = 0;
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

    if (p->radix == 2) {
        throughput = (double) p->K / (p->Kp+p->WS) * p->f/p->n_hi/cp_scaling;
    }
    else if (p->radix == 4) {
        throughput = (double) 2 * p->K / (p->Kp+p->WS) * p->f/p->n_hi/cp_scaling/cp_incr_rad4;
    }
    else if (p->radix == 8) {
        throughput = (double) 3 * p->K / (p->Kp+p->WS) * p->f/p->n_hi/cp_scaling/cp_incr_rad8;
    }
    else {
        throughput = (double) 4 * p->K / (p->Kp+p->WS) * p->f/p->n_hi/cp_scaling/cp_incr_rad16;
    }

    return throughput;
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
                // TEMPORARY: Max-Log-MAP IMPLEMENTATION
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
// p: ptr to model parameters data structure
double get_siso_log_ge_area(Bitwidth *bitwidth, Param *p) {
    double siso_ge_area;
    double bmu_scaling = (double) bitwidth->branch / p->log_ref_bits;
    double pmu_scaling = (double) bitwidth->state / p->log_ref_bits;
    double sou_scaling = pmu_scaling;

    double bmu_area = 2 * get_bmu_ge(p) * bmu_scaling;
    double pmu_area;
    double sou_area = get_sou_ge(p) * sou_scaling;

    // MAX-Log-MAP
    if (p->algorithm == 0) {
        pmu_area = 2 * get_pmu_ge(p) * pmu_scaling;
    }
    // Local-SOVA
    else if (p->algorithm == 1) {
        // Just the backward PMU is different
        pmu_area = get_pmu_ge(p) * pmu_scaling;
        p->algorithm = 0;
        pmu_area += get_pmu_ge(p) * pmu_scaling;
        p->algorithm = 1;
    }

    siso_ge_area = bmu_area + pmu_area + sou_area;
    
    return siso_ge_area;
}

// bitwidth: ptr to bitwidth data structure
// p: ptr to model parameters data structure
double get_turbo_log_ge_area(Bitwidth *bitwidth, Param *p) {
    return (double) p->K / p->Kp * get_siso_log_ge_area(bitwidth, p);
}