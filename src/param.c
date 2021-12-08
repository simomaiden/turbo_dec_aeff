#include <stdio.h>
#include <string.h>
#include "param.h"


// p: ptr to model parameters data structure
void set_tech_scaling_factor(Param *p) {
    if (p->tech == 65) {
        p->tech_scaling = 1;
    }
    else if (p->tech == 28) {
        p->tech_scaling = 4.6;
    }
    else if (p->tech == 7) {
        p->tech_scaling = 25;
    }
}

// filename: input parameters file name
// p: ptr to model parameters data structure
// expl_p: ptr to exploration parameters data structure
// rel_p ptr to reliability parameters results data structure
void initialize_parameters(char filename[], Param *p, ExplorationParam *expl_p, ReliabilityParam *rel_p) {
    FILE *fp;
    char line[50];

    fp = fopen(filename, "r");

    // Collecting informations from the input file
    while(!feof(fp)) {
        fscanf(fp, "%s", line);

        if (line[0] != '/' && strlen(line) > 1) {
            if (strncmp(line, "K=", 2) == 0) {
                sscanf(line, "K=%d\n", &p->K);
            }
            else if (strncmp(line, "Kp=", 3) == 0) {
                sscanf(line, "Kp=%d\n", &p->Kp);
            }
            else if (strncmp(line, "WS=", 3) == 0) {
                sscanf(line, "WS=%d\n", &p->WS);
            }
            else if (strncmp(line, "w=", 2) == 0) {
                sscanf(line, "w=%d\n", &p->w);
            }
            else if (strncmp(line, "f=", 2) == 0) {
                sscanf(line, "f=%d\n", &p->f);
            }
            else if (strncmp(line, "n_hi=", 5) == 0) {
                sscanf(line, "n_hi=%d\n", &p->n_hi);
            }
            else if (strncmp(line, "K_range=", 8) == 0) {
                sscanf(line, "K_range=%d,%d,%d\n", &expl_p->K[0], &expl_p->K[1], &expl_p->K[2]);
            }
            else if (strncmp(line, "Kp_range=", 9) == 0) {
                sscanf(line, "Kp_range=%d,%d,%d\n", &expl_p->Kp[0], &expl_p->Kp[1], &expl_p->Kp[2]);
            }
            else if (strncmp(line, "WS_range=", 9) == 0) {
                sscanf(line, "WS_range=%d,%d,%d\n", &expl_p->WS[0], &expl_p->WS[1], &expl_p->WS[2]);
            }
            else if (strncmp(line, "w_range=", 8) == 0) {
                sscanf(line, "w_range=%d,%d,%d\n", &expl_p->w[0], &expl_p->w[1], &expl_p->w[2]);
            }
            else if (strncmp(line, "th_err=", 7) == 0) {
                sscanf(line, "th_err=%d\n", &rel_p->th_err);
            }
            else if (strncmp(line, "log_err=", 8) == 0) {
                sscanf(line, "log_err=%d\n", &rel_p->log_err);
            }  
            else if (strncmp(line, "mem_err=", 8) == 0) {
                sscanf(line, "mem_err=%d\n", &rel_p->mem_err);
            }
            else if (strncmp(line, "s_file=", 7) == 0) {
                sscanf(line, "s_file=%s\n", p->s_filename);
            }
            else if (strncmp(line, "m_file=", 7) == 0) {
                sscanf(line, "m_file=%s\n", p->m_filename);
            }
            else if (strncmp(line, "av_area=", 8) == 0) {
                sscanf(line, "av_area=%lf\n", &p->av_area);
            }
            else if (strncmp(line, "tech=", 5) == 0) {
                sscanf(line, "tech=%dnm\n", &p->tech);
                set_tech_scaling_factor(p);
            }
            else if (strncmp(line, "algorithm=", 10) == 0) {
                sscanf(line, "algorithm=%d\n", &p->algorithm);
            }
        }
    }

    fclose(fp);

    // Default Parameters
    p->log_ref_bits = 8;
    p->cp_ref_bits = 11;
    p->real_cp = 1;
    p->ge_area = 1.44 / p->tech_scaling;
    p->GE_rca = 41;
    p->GE_rcs = 47;
    p->GE_cs2 = 47;
    p->GE_cs4_f = 224;
    p->GE_cs8_f = 900;
    p->pmu_model = 0;

    p->ge_cell = 0.36;
    p->min_words = 32;
    p->ge_synth_mem = 9.5;

    p->GE_crbar = 37740;
    p->crbar_ref_bits = 7;
    p->banks_ref = 64;
}