
#include "bs_post_processing.h"
#include "cluster.h"
#include "handle_param.h"
#include "list.h"
#include "image_loc.h"

/// Get the foreground rectangle coordinance through cluster
/// Two information: K = rect_width/(rect_height+win_height), F = Num_foreground_in_rect/Num_pixels_in_rect
/// \param[in] output_data : the data contain the foreground data
/// \param[in] width : the image width
/// \param[in] height : the image height
/// \param[in] output_mode : the image output_mode, 0 for gray, 1 for color
/// \param[in] cluster_dist_threshold : the distance threshold to define a cluster, using abs(x1-x0)+abs(y1-y0)
/// \param[in] rect_threshold : the rectangle size threshold, if size is smaller than this, then eliminate it
/// \param[in] save_mode : if equal to 4, then save the coordinance in file
/// \param[out] bs_bbox : the results rectangle vector after clustering

void find_rect_region(unsigned char* output_data, int width, int height, 
                      int output_mode, int cluster_dist_thresh, int rect_thresh, 
                      vector<Rect>& bs_bbox) 
{
    int i, j;
    Rect r;
    vector<Rect> foundRect;    
    struct Cluster *cluster;
    struct Array imageArray;
    list_iterator cluster_list_head, *list;

    imageArray.nx = width;
    imageArray.ny = height;
    if (output_mode == 0)
        imageArray.size = 1;
    else imageArray.size = 3;
    imageArray.data = (char *) output_data;
    get_clusters(&imageArray, cluster_dist_thresh, &cluster_list_head);
   
    list_for_each(list, &cluster_list_head) {
        cluster = list_entry(list, struct Cluster, cluster_list);
        r.x = cluster->minX;
        r.y = cluster->minY;
        r.width = cluster->maxX - cluster->minX;
        r.height = cluster->maxY - cluster->minY;           
        foundRect.push_back(r);        
    }
    
    for (i = 0; i < foundRect.size(); i++) {
        r = foundRect[i]; 
        if (rect_thresh > 1) {
            if (r.width < rect_thresh || r.height < (rect_thresh * 2)) 
                continue;
            for (j = 0; j < foundRect.size(); j++) {
                if ((j != i) && (r & foundRect[j]) == r)
                    break;
            }            
            if (j == foundRect.size())                 
                bs_bbox.push_back(r);                
        }
        else bs_bbox.push_back(r);
    }
    
    free_clusters(&cluster_list_head);
}

/// To get potential number of people at most according to f and k value
/// if F < 0.3 || K < 0.6, n = 0
/// if F < 1.5 || K < 1.1, n = 1
/// if F < 2.8 || K < 1.9, n = 2
/// if F < 5.0 || K < 2.3, n = 3
/// if F < 8.8 || K < 3.8, n = 5, not use
/// param[in] f : F = number of foreground pixels in bs_rect / 1000
/// param[in] k : K = rect_width/wsize_w + rect_height/wsize_h
/// param[out] n : the potential number of people at most 
int get_bs_bbox_n(double f, double k)
{
    int n = 50;
    double f_t[5] = {0.25, 1.6, 3.5, 5.3, 8.6};
    double k_t[5] = {0.6, 1.1, 1.9, 2.3, 3.8};
    if (f < f_t[0] && k < k_t[0]) n = 0;
    else if (f < f_t[1] && k < k_t[1]) n = 1;
    else if (f < f_t[2] && k < k_t[2]) n = 2;
    else if (f < f_t[3] && k < k_t[3]) n = 3;
    else if (f < f_t[4] && k < k_t[4]) n = 5;
    return n;
}
/// Not used now
/// To get f and k information in bs rectangle:
/// K = rect_width/win_width + rect_height/win_height,
/// F = Num_foreground_in_rect / 1000
/// \param[in] output_data : the data contain the foreground data
/// \param[in] bs_bbox : the rectangle vector contain the rect to compute
/// \param[in] width : the image width
/// \param[in] height : the image height
/// \param[in] output_mode : the image output_mode, 0 for gray, 1 for color
/// \param[in] fr : the frame index
/// \param[in] rect_num : the number of rect in bbox
/// \param[in] save : whether to save the ratio info
/// \param[out] bs_bbox_k : K, the rectangle size ratio information
/// \parma[out] bs_bbox_f : F, the foreground pixel ratio information
/// \return predict_p_num : predict pedestrian number in this fr
int bs_ratio_info(unsigned char* output_data, vector<Rect>& bs_bbox, vector<double>& bs_bbox_f, 
                   vector<double>& bs_bbox_k, vector<int>& bs_bbox_n, int fr, int save, 
                   int rect_num, int width, int height, int output_mode)
{
    int n, i, j, s, fore_num;
    int predict_p_num = 0;
    Rect r;
    double f, k;
    int image_offset = 0;
    vector<Rect> copy_bs_bbox;
    copy_bs_bbox.clear();
    for (s = 0; s < rect_num; s++) {
        r = bs_bbox[s];
        copy_bs_bbox.push_back(r);
    }
    bs_bbox.clear();
    for (s = 0; s < rect_num; s++) {
        r = copy_bs_bbox[s];
        k = (double)r.width / wsize_w + (double)r.height / wsize_h;        
        fore_num = 0;
        
        for (i = 0; i < r.height; i++) {
            image_offset = getImageLocOffset(r.tl().x, r.tl().y + i, width);
            for (j = 0; j < r.width; j++) {
                if (output_mode == 1) {
                    if ((output_data[image_offset*3] != 0) || (output_data[image_offset*3+1] != 0) || (output_data[image_offset*3+2] != 0))
                        fore_num++;
                }
                else if (output_data[image_offset] != 0) fore_num++;
                image_offset++;
            }
        }
        f = (double)fore_num / 1000;
        n = get_bs_bbox_n(f, k);
        predict_p_num += n;
        //if (n == 0) continue; // if use these, then miss_rate will increase and fppi decrease
        bs_bbox.push_back(r);
        bs_bbox_f.push_back(f);
        bs_bbox_k.push_back(k);
        bs_bbox_n.push_back(n);
        //if (save) save_bs_bbox_info(r, f, k, n, fr);
    }
    return predict_p_num;
}
