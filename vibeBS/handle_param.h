
#ifndef _HANDLE_PARAMS_
#define _HANDLE_PARAMS_

/* Data type of params */
enum {
    ARG_INT = 0,
    ARG_DOUBLE,
    ARG_STRING,
};

#define MAX_LENGTH 256

/* Data structure to handle params */
struct ArgsConfig {
    char *abbrName;                // The abbreviated name of the argument.
    char *detailDescription;       // Detail description of the argument.
    void *addr;                    // Address of the global variables to store the argument. 
    int type;                      // Type of the argument.
};

/* extern variables definition */
extern char image_list_filename[MAX_LENGTH];
extern char video_filename[MAX_LENGTH];

extern int bs_model;
extern int save_mode;
extern int save_freq;
extern int output_mode;

extern char vibe_model_filename[MAX_LENGTH];
extern int sample_num;
extern int match_threshold;
extern int match_num;
extern int update_speed;

extern int cluster_dist_threshold;
extern int rect_threshold;

extern int wsize_w;
extern int wsize_h;
extern int bsize_w;
extern int bsize_h;
extern int bstride_w;
extern int bstride_h;
extern int csize_w;
extern int csize_h;
extern int nbins;

extern char hog_model_filename[MAX_LENGTH];
extern char gt_path[MAX_LENGTH];
extern char dt_path[MAX_LENGTH];

extern double overlapThreshold;
extern int feat_dimention;
extern int winStrideX;
extern int winStrideY;
extern int paddingX;
extern int paddingY;
extern double scaleratio;
extern double hitThreshold;
extern int groupThreshold;

extern double dt_f_down_threshold;
extern double dt_k_up_threshold;
extern double dt_k_down_threshold;

extern int enlarge_x;
extern int enlarge_y;
//extern int enlarge_width;
//extern int enlarge_height;
extern int shrink_m;
extern double shrink_x;
extern double shrink_y;
extern double shrink_width;
extern double shrink_height;

extern int debug_info;

/* Function */
void usage(int argc, char **argv);
void handleArgs(int argc, char **argv);

#endif