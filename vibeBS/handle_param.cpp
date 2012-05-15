#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "handle_param.h"

/* variables definition */
//input
char image_list_filename[MAX_LENGTH] = "";
char video_filename[MAX_LENGTH] = "";

//background parameters
int bs_model = 1;
int save_mode = 0;
int save_freq = 1;
int output_mode = 0;

//vibe parameters
char vibe_model_filename[MAX_LENGTH] = "";
int sample_num = 20;
int match_threshold = 20;
int match_num = 2;
int update_speed = 16;

//cluster for rectangle
int cluster_dist_threshold = 15;
int rect_threshold = 10;


//hog descriptor parameters
int wsize_w = 64;
int wsize_h = 128;
int bsize_w = 16;
int bsize_h = 16;
int bstride_w = 8;
int bstride_h = 8;
int csize_w = 8;
int csize_h = 8;
int nbins = 9;

//detect file paramters
char hog_model_filename[MAX_LENGTH] = "";
char gt_path[MAX_LENGTH] = "";
char dt_path[MAX_LENGTH] = "";

//detect parameters
double overlapThreshold = 0.5;
int feat_dimention = 3780;
int winStrideX = 8;
int winStrideY = 8;
int paddingX = 32;
int paddingY = 32;
double scaleratio = 1.05;
double hitThreshold = 0.0;
int groupThreshold = 2;

//detect filter parameters
double dt_k_up_threshold = 0.6;
double dt_k_down_threshold = 0.24;
double dt_f_down_threshold =0.14;

//rectangle enlarge and shrink factor before detect
int enlarge_x = 15;
int enlarge_y = 15;
//int enlarge_width = 32;
//int enlarge_height = 32;
int shrink_m = 1;
double shrink_x = 0.2;
double shrink_y = 0.09;
double shrink_width = 0.6;
double shrink_height = 0.8;

int debug_info = 0;

/* Arguments */
static struct ArgsConfig argsConfigs[] = { 
    {"-lstf", " : Image list file to be handled", (void *) image_list_filename, ARG_STRING},
    {"-vidf", " : Video file to be handled", (void *) video_filename, ARG_STRING}, 
    {"\nBackground subtraction parameters:", ""},
    {"-bsM", " :Background subtraction mode. Default 1" \
             "\n\t\t\t 0 for read VibeModel(-vibeM must set);" \
             "\n\t\t\t 1 for Vibe;" \
             "\n\t\t\t 2 for GMM;", 
				(void *) &bs_model, ARG_INT},
    {"-saveM", " : Save mode, every bit denote a mode. Default 0" \
               "\n\t\t\t 0 for no;" \
               "\n\t\t\t bit 1 for origin frame(1);" \
               "\n\t\t\t bit 2 for foreground(2);" \
               "\n\t\t\t bit 3 for rectangle origin(4);" \
               "\n\t\t\t bit 4 for rectangle foreground(8);" \
               "\n\t\t\t bit 5 for detected rectangle coordinance file(16);" \
               "\n\t\t\t bit 6 for vibe model(32); " \
               "\n\t\t\t bit 7 for detailed infomation(64); " \
               "\n\t\t\t bit 8 for result and params infomation(128); " \
               "\n\t\t\t bit 9 for detected small image(256); "\
               "\n\t\t\t bit 10 for ...(512);" ,
				(void *) &save_mode, ARG_INT},
    {"-saveF", " : Save frame frequency. Default 1", (void *) &save_freq, ARG_INT},
//    {"-outputM", " : Binary or color output image. Default 0" \
//                 "\n\t\t\t 0 for binary;" \
//                 "\n\t\t\t 1 for color;", 
//				(void *) &output_mode, ARG_INT},
    {"\nVibe background subtraction parameters:", ""},
    {"-vibeM", " : Vibe model file to load or save", (void *) vibe_model_filename, ARG_STRING},
//    {"-sampleN", " : Sample number in the background model, used only if bsm = 1. Default 20", (void *) &sample_num, ARG_INT},
//    {"-matchT", " : Match distance threshold in the background model, used only if bsm = 1. Default 20", (void *) &match_threshold, ARG_INT},
//    {"-matchN", " : Match number threshold in the background model, used only if bsm = 1. Default 2", (void *) &match_num, ARG_INT},
//    {"-updateS", " : Update speed in the background model, used only if bsm = 1. Default 16", (void *) &update_speed, ARG_INT},
    {"\nCluster for rectangle:", ""},
    {"-clusterT", " : Clusters distance threshold in region extraction. Default 15", (void *) &cluster_dist_threshold, ARG_INT},
    {"-rectangleT", " : Rectangle size threshold in region extraction. Default 10*20", (void *) &rect_threshold, ARG_INT},
//    {"\nHog descriptor parameters:", ""},
//    {"-wsw", " : Detect window width. Default 64", (void *) &wsize_w, ARG_INT},
//    {"-wsh", " : Detect window height. Default 128", (void *) &wsize_h, ARG_INT},
//    {"-bsw", " : Hog block width. Default 16", (void *) &bsize_w, ARG_INT},
//    {"-bsh", " : Hog block height. Default 16", (void *) &bsize_h, ARG_INT},
//    {"-btw", " : Hog block stride width. Default 8", (void *) &bstride_w, ARG_INT},
//    {"-bth", " : Hog block stride height. Default 8", (void *) &bstride_h, ARG_INT},
//    {"-csw", " : Hog cell width. Default 8", (void *) &csize_w, ARG_INT},
//    {"-csh", " : Hog cell height. Default 8", (void *) &csize_h, ARG_INT},
//    {"-nbin", " : Hog number of bins. Default 9", (void *) &nbins, ARG_INT},
    {"\nDetect file parameters:", ""},    
    {"-modf", " : Hog model File to load, if not defined, use default opencvHogDector", (void *) &hog_model_filename, ARG_STRING},
    {"-dtp", ": The path of the detected location and deci stored", (void *) &dt_path, ARG_STRING},
    {"-gtp"," : The path of the groud truth file stored", (void *) &gt_path, ARG_STRING},
    {"\nDetect parameters:", ""},
    {"-ot", " : Overlap Threshold,only used when defined -gtp. Default 50%", (void *) &overlapThreshold, ARG_DOUBLE},
//    {"-fd", " : Hog feature dimention. Default 3780", (void *) &feat_dimention, ARG_INT},    
//    {"-wx", " : Window stride width. Default 8", (void *) &winStrideX, ARG_INT},
//    {"-wy", " : Window stride height. Default 8", (void *) &winStrideY, ARG_INT},
//    {"-px", " : Image padding width. Default 32", (void *) &paddingX, ARG_INT},
//    {"-py", " : Image padding height. Default 32", (void *) &paddingY, ARG_INT},
    {"-sr", " : Detect scale Ratio. Default 1.05", (void *) &scaleratio, ARG_DOUBLE},
    {"-ht", " : Hit threshold. Default 0", (void *) &hitThreshold, ARG_DOUBLE},
//    {"-gt", " : Multi scale rectangle group threshold. Default 2", (void *) &groupThreshold, ARG_INT},
    {"\nDetect filter parameters:", ""},
    {"-dtFdownT", " : Detected rectangle pixel down ratio threshold. Default 0.14", (void *) &dt_f_down_threshold, ARG_DOUBLE},
    {"-dtKupT", " : Detected rectangle size up ratio threshold. Default 0.6", (void *) &dt_k_up_threshold, ARG_DOUBLE},
    {"-dtKdownT", " : Detected rectangle size down ratio threshold. Default 0.24", (void *) &dt_k_down_threshold, ARG_DOUBLE},
    {"\nRectangle enlarge and shrink factor before detect:", ""},
    {"-rlx", " : Rectangle coordinance x enlarge factor. Default 15", (void *) &enlarge_x, ARG_INT},
    {"-rly", " : Rectangle coordinance y enlarge factor. Default 15", (void *) &enlarge_y, ARG_INT},
    //{"-rlw", " : Rectangle coordinance width enlarge factor. Default 32", (void *) &enlarge_width, ARG_INT},
    //{"-rlh", " : Rectangle coordinance height enlarge factor. Default 32", (void *) &enlarge_height, ARG_INT},
    {"-rsm", " : Rectangle shrink mode, 1 for Normal, 2 for CAVIAR. Default 1", (void *) &shrink_m, ARG_INT},
//    {"-rsx", " : Rectangle coordinance x shrink factor. Default 0.2", (void *) &shrink_x, ARG_DOUBLE},
//    {"-rsy", " : Rectangle coordinance y shrink factor. Default 0.09", (void *) &shrink_y, ARG_DOUBLE},
//    {"-rsw", " : Rectangle coordinance width shrink factor. Default 0.6", (void *) &shrink_width, ARG_DOUBLE},
//    {"-rsh", " : Rectangle coordinance height shrink factor. Default 0.8", (void *) &shrink_height, ARG_DOUBLE},
    {"\nDebug parameters:", ""},
    {"-debug", " : Whether to print some information. Default 0" \
               "\n\t\t\t 0 for no;" \
               "\n\t\t\t 1 for yes;", (void *) &debug_info, ARG_INT},
    {NULL, NULL, NULL, 0}
};

void usage(int argc, char **argv)
{
    struct ArgsConfig *cfg;
    printf("\nUsage: %s list filename | video filename [options]\n", argv[0]);
    
    for (cfg = argsConfigs; cfg->abbrName; cfg++) {
        printf("\t%s\t%s\n", cfg->abbrName, cfg->detailDescription);
    }
    exit(1);
}

void handleArgs(int argc, char **argv)
{
    int i;
    struct ArgsConfig *cfg;

    // Only print usage info if run this program with "-h" or "--help"
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            usage(argc, argv);
        }
    }
    for (cfg = argsConfigs; cfg->abbrName; cfg++) {
        for (i = 1; i < argc; i++) {
            if (strcmp(cfg->abbrName, argv[i])) continue;
            break;
        }
        if (i == argc) { // Not found a match one
            continue;
        }
        // Found match one. Get value from next.
        i++;
        if (i == argc) { // Not value follow up
            printf("Miss value for %s\n", cfg->abbrName);
            usage(argc,argv);
        }
        switch (cfg->type) {
        case ARG_INT:
            *((int *) cfg->addr) = atoi(argv[i]);
            break;
        case ARG_DOUBLE:
            *((double *) cfg->addr) = atof(argv[i]);
            break;
        case ARG_STRING:
            strcpy((char *) cfg->addr, argv[i]);
            break;
        default:
            printf("Never come here!\n");
            assert(0);
        }
    }
    return;
}
