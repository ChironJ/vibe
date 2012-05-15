#include "image_loc.h"

int getRandomNumber(int begin, int end)
{
    int rand_num;
    rand_num = (rand() % (end - begin)) + begin;
    return rand_num;
}

int getRandomNeighbor9Coordinate(int ori, int max)
{
    int rand_neighbor;
    rand_neighbor = getRandomNumber(-1, 2);
    rand_neighbor += ori;
    if (rand_neighbor < 0) rand_neighbor = 0;
    if (rand_neighbor >= max) rand_neighbor = max - 1;
    return rand_neighbor;
}

// To get the location offset in the background model according to
// the location of current pixel in image.
/*
For gray:
    Assume width = 3, height = 3, nSample = 4,
    then the image data is -------------------        -------
                           |(0,0)|(1,0)|(2,0)|        |0|1|2|
                           -------------------        -------
                           |(0,1)|(1,1)|(2,1)| -->    |3|4|5|
                           -------------------        -------
                           |(0,2)|(1,2)|(2,2)|        |6|7|8|
                           -------------------        -------
    and the correspond bg_samples is
                            (0,0)              (1,0)               (2,0)
                          |0|1|2|3|          |4|5|6|7|           |8|9|10|11|
                            (0,1)              (1,1)               (2,1)
                          |12|13|14|15|      |16|17|18|19|       |20|21|22|23|
                            (0,2)              (1,2)               (2,2)
                          |24|25|26|27|      |28|29|30|31|       |32|33|34|35|
                
For color:
    then the image data is -------------------        ----------------------------
                           |(0,0)|(1,0)|(2,0)|        | 0, 1, 2| 3, 4, 5| 6, 7, 8|
                           -------------------        ----------------------------
                           |(0,1)|(1,1)|(2,1)| -->    | 9,10,11|12,13,14|15,16,17|
                           -------------------        ----------------------------
                           |(0,2)|(1,2)|(2,2)|        |18,19,20|21,22,23|24,25,26|
                           -------------------        ----------------------------
    and the correspond bg_samples is  
                                (0,0)                                            (1,0)                                    (2,0)
                    | 0, 1, 2| 3, 4, 5| 6, 7, 8| 9,10,11|        |12,13,14|15,16,17|18,19,20|21,22,23|        |24,25,26| 27, 28, 29| 30, 31, 32| 33, 34, 35|
                                (0,1)                                            (1,1)                                    (2,1)
                    |36,37,38|39,40,41|42,43,44|45,46,47|        |48,49,50|51,52,53|54,55,56|57,58,59|        |60,61,62| 63, 64, 65| 66, 67, 68| 69, 70, 71|
                                (0,2)                                            (1,2)                                    (2,2)
                    |72,73,74|75,76,77|78,79,80|81,82,83|        |84,85,86|87,88,90|91,92,93|94,95,96|        |97,98,99|100,101,102|103,104,105|106,107,108|
*/
int getSampleLocOffset(int x, int y, int width, int nSample, int index)
{
    return y * width * nSample + x * nSample + index;
}

int getImageLocOffset(int x, int y, int width)
{
    return y * width + x;
}

/// Copy the image_data to output_data according to nChannel
void imageData_2_outputData(unsigned char* image_data, unsigned char * output_data, int output_offset, int image_offset, int nChannel)
{
    if (nChannel == 1)
        output_data[output_offset] = image_data[image_offset];
    else if (nChannel == 3) {
        output_data[output_offset * 3] = image_data[image_offset * 3];
        output_data[output_offset * 3 + 1] = image_data[image_offset * 3 + 1];
        output_data[output_offset * 3 + 2] = image_data[image_offset * 3 + 2];
    }
}

/// Get the neighbor image location, and avoid the edge
/// \param[in] width height : the image width and height
/// \param[in] x y : the image location(x, y)
/// \param[out] nx1 : the image neibor (x+1,y)
/// \param[out] nx2 : the image neibor (x-1,y)
/// \param[out] ny1 : the image neibor (x, y+1)
/// \param[out] ny2 : the image neibor (x, y-1)
void getNeighborLoc(int width, int height, int x, int y, int *nx1, int *nx2, int *ny1, int *ny2)
{
    int x1, x2, y1, y2;
     x1 = x + 1; if (x1 > width) x1 = width;
     x2 = x - 1; if (x2 < 0) x2 = 0;
     y1 = y + 1; if (y1 > height) y1 = height;
     y2 = y - 1; if (y2 < 0) y2 = 0;
     *nx1 = getImageLocOffset(x1, y, width);
     *nx2 = getImageLocOffset(x2, y, width);
     *ny1 = getImageLocOffset(x, y1, width);
     *ny2 = getImageLocOffset(x, y2, width);
}