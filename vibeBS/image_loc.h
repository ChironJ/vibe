#ifndef __IMAGE_LOC__
#define __IMAGE_LOC__

#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_COLOR_BACKGROUND   0
#define DEFAULT_COLOR_FOREGROUND 255

int getRandomNumber(int begin, int end);
int getRandomNeighbor9Coordinate(int ori, int max);

int getSampleLocOffset(int x, int y, int width, int nSample, int index);
int getImageLocOffset(int x, int y, int width);

void imageData_2_outputData(unsigned char* image_data, unsigned char* output_data, int output_offset, int image_offset, int nChannel);

void getNeighborLoc(int width, int height, int x, int y, int *nx1, int *nx2, int *ny1, int *ny2);

#endif
