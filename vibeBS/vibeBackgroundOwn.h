/*
  Example of a code (in c99) for your main file:
  ----------------------------------------------
  #include "vibe-background.h"
  
  int main(int argc, char **argv){
     
     // Acquire your first (grayscale) image
     // nb: stride is te number of bytes from the start of one row of the image  
     //     to the start of the next row.
     unsigned char *image_data = acquire_image(stream);
     int width = get_image_width(stream);
     int height = get_image_height(stream);
     int stride = get_image_stride(stream);

     // Allocate another image to store the resulting segmentation map
     unsigned char *segmentation_map = malloc(stride * height);
  
     // Allocate the model and initialize it with the first image
     vibeModelCreate_8u(model, image_data, width, height, stride);
  
     // Process all the following frames of your stream
     // nb. the results will be stored in *segmentation_map
     while(!finished(stream)){
        image_data = acquire_image(stream);
        vibeModelUpdate_8u(model, image_data, segmentation_map);
     }

     // Cleanup allocated memory
     vibeModelFree(model);
  }
*/

#ifndef __VIB_BACKGROUND_OWN__
#define __VIB_BACKGROUND_OWN__

#include <stdlib.h>
#include <stdio.h>

typedef struct vibeModel
{
	int width;        //image width
	int height;       //image height
    int nChannel;     //number of image channel
	int nSample;      //number of samples in per pixel's background model
	int nMatch;       //number of close samples for being part of the background
	int threshold;    //radius of the sphere, 
			  //the distance to compare current pixel with samples in its background model
	int upSpeed;      //random of update speed
	unsigned char* bg_samples;	//background model
} vibeModel;

vibeModel* VibeModelInit(int width, int height, int nChannel, unsigned char *image_data);

void VibeModelUpdate(vibeModel* model, unsigned char* data, unsigned char* output);

void VibeModelFree(vibeModel** model);
void VibeModelSave(vibeModel* model);
vibeModel* VibeModelRead(FILE* vibe_model_file);
int vibeModelSaveParameters(const vibeModel *model);

#endif
