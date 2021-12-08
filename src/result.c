#include "result.h"
#include <stdio.h>

// arch: ptr to [ArchData] struct
// fp: file pointer
void print_arch_details(ArchData *arch, FILE *fp) {
    // Computing the logic and memory area percentage contributions
    int log_perc = (int) (double)(arch->log_area / (arch->log_area + arch->mem_area) * 100);
    int mem_perc = 100 - log_perc;

    // Printing the data
    fprintf(fp, "Radix: %d \n", arch->radix);
    fprintf(fp, "K: %d \n", arch->K);
    fprintf(fp, "Kp: %d \n", arch->Kp);
    fprintf(fp, "WS: %d \n", arch->WS);
    fprintf(fp, "w: %d \n", arch->w);
    fprintf(fp, "Throughput: %.3lf [Gb/s]\n", arch->throughput);
    fprintf(fp, "Logic Area: %.3lf (%d%c) [mm^2]\n", arch->log_area, log_perc, '%');
    fprintf(fp, "Memory Area: %.3lf (%d%c) [mm^2]\n", arch->mem_area, mem_perc, '%');
    fprintf(fp, "Area Efficiency: %.3lf [Gb/s/mm^2]\n", arch->aeff);
}

// arch: array of [ArchData] struct
// filename: path for the results file
// open option: option to open the file (w, r, a)
void save_single_radix_analysis(ArchData arch[], char filename[], char open_option[]) {
    FILE *fp;
    int i;

    fp = fopen(filename, open_option);
    fprintf(fp, "/// BEST RADIX-ORDER ANALYSIS ///\n\n");
    fprintf(fp, "-----------------------------------\n");
    fprintf(fp, "Radix\tAeff [Gb/s/mm^2]\n");

    // Printing the efficiency table
    for (i = 0; i < 4; i++) {
        fprintf(fp, "%d\t\t%.3lf\n", arch[i].radix, arch[i].aeff);
    }
    fprintf(fp, "-----------------------------------\n");

    // Printing details
    fprintf(fp, "\n/// DETAILS ///\n\n");
    for (i = 0; i < 4; i++) {
        fprintf(fp, "-----------------------------------\n");
        print_arch_details(&arch[i], fp);
        fprintf(fp, "-----------------------------------\n");
        fprintf(fp, "\n\n");
    }
    fclose(fp);
}

// arch: array of [ArchData] struct
// archnum: number of architectures in the array
// filename: path for the results file
// open option: option to open the file (w, r, a)
void save_dataset(ArchData arch[], int arch_num, char filename[], char open_option[]) {
    FILE *fp;
    int i;

    fp = fopen(filename, open_option);
    fprintf(fp, "Radix, K, Kp, WS, w, th, tot_area, aeff, th_on_area, pmap_num\n");
    for (i = 0; i < arch_num; i++) {
        // Print order: <Radix, K, Kp, WS, w, th, tot_area, aeff, th_on_area, pmap_num>
        fprintf(fp, "%d,%d,%d,%d,%d,%lf,%lf,%lf,%lf,%d\n", arch[i].radix, arch[i].K, arch[i].Kp, arch[i].WS, arch[i].w, arch[i].throughput, arch[i].total_area, arch[i].aeff, arch[i].throughput_on_area, arch[i].pmap_num);
    }

    fclose(fp);
}

// rel_p ptr to reliability parameters results data structure
// filename: output file name
// open option: option to open the file (w, r, a)
void save_reliability_analysis(ReliabilityResult *rel_res, char filename[], char open_option[]) {
    FILE *fp;

    fp = fopen(filename, open_option);

    fprintf(fp, "/// RELIABILITY RESULTS ///\n\n");
    fprintf(fp, "-----------------------------------\n");
    fprintf(fp, "Radix\t%c Cases\n", '%');
    fprintf(fp, "2\t\t%.2lf\n", rel_res->rad2_perc);
    fprintf(fp, "4\t\t%.2lf\n", rel_res->rad4_perc);
    fprintf(fp, "8\t\t%.2lf\n", rel_res->rad8_perc);
    fprintf(fp, "16\t\t%.2lf\n", rel_res->rad16_perc);
    fprintf(fp, "-----------------------------------\n");

    fclose(fp);
}