//#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include "vibeBackgroundOwn.h"
#include "image_loc.h"
#include "handle_param.h"

int vibeModelSaveParameters(const vibeModel *model)
{
    FILE *info_file = NULL;
    info_file = fopen("./pedestrian_in_video.info", "a+");
    if (!info_file) {
        fprintf(stderr, "ERROR: Can not open info_file\n");
        return -1;
    }
    fprintf(info_file, "\nThe vibe background subtraction parameters are:\n");
    fprintf(info_file, "\tthe number of samples in the background model is %d\n", model->nSample);
    fprintf(info_file, "\tthe number of close samples to be part of background is %d\n", model->nMatch);
    fprintf(info_file, "\tthe threshold to judge whether the pixel belongs to background is %d\n", model->threshold);
    fprintf(info_file, "\tthe speed of updating the background model is %d\n", model->upSpeed);
    fclose(info_file);
    return 0;
}

//initialize the background model with the first frame
vibeModel* VibeModelInit(int width, int height, int nChannel, unsigned char* image_data)
{
    int x, y, xNG, yNG, index;
    int image_offset = 0, sample_offset = 0;
    vibeModel* vbM = new(vibeModel);
    vbM->width = width;
    vbM->height = height;
    vbM->nChannel = nChannel;
    vbM->nSample = sample_num;
    vbM->threshold = match_threshold;
    vbM->nMatch = match_num;
    vbM->upSpeed = update_speed;
    vbM->bg_samples = (unsigned char *) malloc(width * height * nChannel * vbM->nSample);    
    //memset(vbM->bg_samples, 0, width * height * nChannel * vbM->nSample);
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            for (index = 0; index < sample_num; index++) {
                yNG = getRandomNeighbor9Coordinate(y, height);
                xNG = getRandomNeighbor9Coordinate(x, width);
                sample_offset = getSampleLocOffset(x, y, width, sample_num, index);
                image_offset = getImageLocOffset(xNG, yNG, width);
                imageData_2_outputData(image_data, vbM->bg_samples, sample_offset, image_offset, nChannel);                
            }
        }
    }
    //srand((unsigned int) time(NULL));
    return vbM;
}

//classify every pixel in the current frame according its background model
//update the chosen pixel's and its random neibor's background model randomely 
void VibeModelUpdate(vibeModel* model, unsigned char* image, unsigned char* output)
{
    int width = model->width;
    int height = model->height;
    int nChannel = model->nChannel;
    int nSample = model->nSample;
    int nMatch = model->nMatch;
    int threshold = model->threshold;
    int upSpeed = model->upSpeed;
    int image_offset = 0;
    int sample_offset = 0;
    unsigned char* samples = model->bg_samples;
    int count = 0, index = 0, distR = 0, distG = 0, distB = 0, dist = 0;
    int x, y, xNG, yNG;

    image_offset = -1;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            image_offset++;
            //1. compare pixel to background model
            count = 0; index = 0;
            while ((count < nMatch) && (index < nSample)) {
                //Euclidean distance computation
                sample_offset = getSampleLocOffset(x, y, width, nSample, index);
                if (nChannel == 1) dist = abs(image[image_offset] - samples[sample_offset]);
                else if (nChannel == 3) {
                    distR = abs(image[image_offset * 3] - samples[sample_offset * 3]); 
                    if (output_mode == 1) {
                        distG = abs(image[image_offset * 3 + 1] - samples[sample_offset * 3 + 1]);
                        distB = abs(image[image_offset * 3 + 2] - samples[sample_offset * 3 + 2]);
                        dist = distR * distR + distG * distG + distB * distB;
                        dist = (int) sqrt((double) dist / 3);
                    }
                    else dist = distR;
                }
                
                if (dist < threshold) count++;
                index++;
            }
            //2. classify pixel and update model
            if (count >= nMatch) {
                //store that image[x][y] belongs to background
                if (nChannel == 3 && (output_mode == 1)) {
                    output[image_offset*3] = DEFAULT_COLOR_BACKGROUND;
                    output[image_offset*3+1] = DEFAULT_COLOR_BACKGROUND;
                    output[image_offset*3+2] = DEFAULT_COLOR_BACKGROUND;
                }
                else output[image_offset] = DEFAULT_COLOR_BACKGROUND;
                //3. update current pixel model
                //get random number between 0 and fi-1(update speed, time sampling)
                int rand = getRandomNumber(0, upSpeed);
                if (rand == 0) {//random subsampling
                    //replace randomly chosen sample
                    rand = getRandomNumber(0, nSample);
                    sample_offset = getSampleLocOffset(x, y, width, nSample, rand);
                    imageData_2_outputData(image, samples, sample_offset, image_offset, nChannel);                    
                }
                //4. update neighboring pixel model
                rand = getRandomNumber(0, upSpeed);
                if (rand == 0) {//random subsampling
                    //choose neighboring pixel randomly
                    xNG = getRandomNeighbor9Coordinate(x, width);
                    yNG = getRandomNeighbor9Coordinate(y, height);
                    while ((xNG == x) && (yNG == y)) { // the real neighbor, exclude (x,y) itself
                        xNG = getRandomNeighbor9Coordinate(x, width);
                        yNG = getRandomNeighbor9Coordinate(y, height);
                    }
                    //replace randomly choosen sample
                    rand = getRandomNumber(0, nSample);
                    sample_offset = getSampleLocOffset(xNG, yNG, width, nSample, rand);
                    imageData_2_outputData(image, samples, sample_offset, image_offset, nChannel);
                }
             } else { //count < #min
            //store that image[x][y] belongs to foreground
                if (output_mode == 0) // for gray output
                    output[image_offset] = DEFAULT_COLOR_FOREGROUND;
                else if (output_mode == 1)  // for color output
                    imageData_2_outputData(image, output, image_offset, image_offset, nChannel); 
            }// end of else for count < #min
        }// end of for x loop
    }// end of for y loop
}

void VibeModelFree(vibeModel** model)
{    
    free((*model)->bg_samples);
    delete (*model);
    (*model) = NULL;
}

void VibeModelSave(vibeModel* model)
{
    FILE *vibeModelFile = NULL;
    int i, data_num;
    unsigned char* ptr;
    vibeModelFile = fopen("../workspace/bs_model/vibe.model","wb");
    fprintf(vibeModelFile,"%d", model->width);
    fprintf(vibeModelFile," %d", model->height);
    fprintf(vibeModelFile," %d", model->nChannel);
    fprintf(vibeModelFile," %d", model->nSample);
    fprintf(vibeModelFile," %d", model->nMatch);
    fprintf(vibeModelFile," %d", model->threshold);
    fprintf(vibeModelFile," %d\n", model->upSpeed);
    data_num = model->width * model->height * model->nSample * model->nChannel;
    ptr = model->bg_samples;
    for (i = 0; i < data_num; i++) {
        fprintf(vibeModelFile, "%d ", *ptr);
        ptr++;
    }
    fclose(vibeModelFile);
    return;
}
vibeModel* VibeModelRead(FILE* vibe_model_file)
{
    vibeModel* vbM = new(vibeModel);
    unsigned char *ptr;
    int i, data_num, tmp;

    fscanf(vibe_model_file, "%d", &vbM->width);
    fscanf(vibe_model_file, " %d", &vbM->height);
    fscanf(vibe_model_file, " %d", &vbM->nChannel);
    fscanf(vibe_model_file, " %d", &vbM->nSample);
    fscanf(vibe_model_file, " %d", &vbM->nMatch);
    fscanf(vibe_model_file, " %d", &vbM->threshold);
    fscanf(vibe_model_file, " %d\n", &vbM->upSpeed);

    data_num = vbM->width * vbM->height * vbM->nChannel * vbM->nSample;
    vbM->bg_samples = (unsigned char *) malloc(data_num);
    ptr = vbM->bg_samples;
    for (i = 0; i < data_num; i++) {
        fscanf(vibe_model_file, "%d ", &tmp);
        *ptr = (unsigned char) tmp;
        ptr++;
    }
    
    return vbM;
}