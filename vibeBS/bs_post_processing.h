#ifndef _BS_POST_PROCESS_
#define _BS_POST_PROCESS_

#include "cvaux.h"
#include "highgui.h"

using namespace cv;
using namespace std;

void find_rect_region(unsigned char *output_data, int width, int height, 
                      int output_mode, int cluster_dist_thresh, int rect_thresh, 
                      vector<Rect>& bs_bbox);

int bs_ratio_info(unsigned char* output_data, vector<Rect>& bs_bbox, vector<double>& bs_bbox_f, 
                   vector<double>& bs_bbox_k, vector<int>& bs_bbox_n, int fr, int save, 
                   int rect_num, int width, int height, int output_mode);



#endif