#ifndef _PARAM_H
#define _PARAM_H

#define DEFAULT_CHANNEL_BITWIDTH 6

// Model parameters structure
typedef struct paramStruct {
    int algorithm;          // Decoding algorithm: [0]: Max-Log-MAP, [1]: Local-SOVA
    int K;                  // Frame-Size
    int Kp;                 // Sub-Frame-Size
    int WS;                 // Window-Size
    int f;                  // Radix-2 Frequency [Hz]
    int f_max;              // Max. Frequency [Hz]
    int n_hi;               // Half-Iterations number
    int radix;              // Radix order
    int w;                  // Channel Bitwidth
    double av_area;         // Maximum available area for Decoder-level parallelism
    int tech;               // Technology [nm]
    double pow_per_f_area;  // Power per unit of frequency and area [W/MHz/mm^2]
    int add;                // Adders model [0] RCA, [1] CLA.
    int recomp;             // Alpha metrics memory reduction (recomputation) [%]

    double tech_scaling;    // Technology scaling factor (65nm as reference)
    int log_ref_bits;       // Reference bitwidth for logic area
    int cp_ref_bits;        // Critical path reference bits
    int real_cp;            // [1]: Use realistic cp increment factors, [0]: Same critical path
    double ge_area;         // Area for a Gate Equivalent [um^2]
    double GE_rca;          // GE Ripple-Carry-Adder
    double GE_rcs;          // GE Ripple-Carry-Subtractor
    double GE_cs2;          // GE Compare-and-Select-2
    double GE_cs4_f;        // GE Compare-and-Select-4 fast
    double GE_cs8_f;        // GE Compare-and-Select-8 fast
    int pmu_model;          // [0]: CS4-fast, [1]: CS8-fast

    double ge_cell;         // GE per standard memory cell
    int min_words;          // Minimum words for library memories
    double ge_synth_mem;    // Average GE/bit for synthesized memories

    double GE_crbar;        // GE Crossbar
    int crbar_ref_bits;     // Crossbar reference bitwidth
    int banks_ref;          // Crossbar reference number of banks (partitions)

    char s_filename[50];    // Single analysis results filename
    char m_filename[50];    // Muliple analysis results filename
} Param;

// Data bitwidth structure
typedef struct bitwidthStruct {
    int channel;            // Channel LLR
    int branch;             // Branch metrics
    int state;              // State metrics
    int extrinsic;          // Extrinsic information
} Bitwidth;

// Memory Area data structure
typedef struct memAreaStruct {
    double in_frame;
    double alpha;
    double nii;
    double extrinsic;
    double perm;
    double crossbar;
    double total;
} MemArea;

// Logic Area data structure
typedef struct logAreaStruct {
    double bmu;
    double pmu;
    double sou;
    double total;
} logArea;

// Architecture results data structure
typedef struct architectureDataStruct {
    int radix;                      // Radix-order
    int K;                          // Frame-Size
    int Kp;                         // Sub-Frame-Size
    int WS;                         // Window-Size
    int w;                          // Channel LLRs bitwidth
    double throughput;              // Throughput [Gb/s]
    double latency;                 // Latency [us]
    double power;                   // Power [W]
    double power_density;           // Power density [W/mm^2]
    double energy_eff;              // Energy Efficiency [pJ/bit/mm^2]
    logArea log_area;               // Logic Area [mm^2]
    MemArea mem_area;               // Memory Area [mm^2]
    double total_area;              // Total Area [mm^2]
    double aeff;                    // Area efficiency [Gb/s/mm^2]
    double throughput_on_area;      // Throughput on a given area [Gb/s]
    int pmap_num;                   // Number of PMAP architectures in the given area
} ArchData;

// Exploration parameters (multiple analysis) data structure
typedef struct explorationParamStruct {
    int K[3];       // Frame-Size [0]: Start, [1]: Stop, [2]: Step
    int Kp[3];      // Sub-Frame-Size [0]: Start, [1]: Stop, [2]: Step
    int WS[3];      // Window-Size [0]: Start, [1]: Stop, [2]: Step
    int w[3];       // LLR bitwidth [0]: Start, [1]: Stop, [2]: Step
} ExplorationParam;

// Reliability parameters data structure
typedef struct reliabilityParamStruct {
    int log_err;    // Error on logic area [%]
    int mem_err;    // Error on memory area [%]
    int th_err;     // Error on throughput [%]
} ReliabilityParam;

// Reliability results data structure
typedef struct realibilityResultStruct {
    double rad2_perc;   // Percentage of radix-2 solutions
    double rad4_perc;   // Percentage of radix-4 solutions
    double rad8_perc;   // Percentage of radix-8 solutions
    double rad16_perc;  // Percentage of radix-16 solutions
} ReliabilityResult;

void initialize_parameters(char filename[], Param *p, ExplorationParam *expl_p, ReliabilityParam *rel_p);

#endif