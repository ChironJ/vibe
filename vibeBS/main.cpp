#include "vibeBackgroundOwn.h"
#include "bs_post_processing.h"
#include "cv.h"
#include "highgui.h"
#include <iostream>
using namespace std;

int main(int argc, char *argv[])
{
	vibeModel* vbM = NULL;
	unsigned char*  image_data = NULL; // image data in current image
    unsigned char*  output_data = NULL; // image data after background subtraction
	vector<cv::Rect> bs_bbox; // contain rectangle regions after post processing
	string file;
	if (argc == 1)
	{
		cout << "input video path:"<< endl;
		cin >> file;
	}
	else
		file = argv[1];
	CvCapture* video = cvCaptureFromFile(file.c_str());
	IplImage *frame = cvQueryFrame(video);
	int width = frame->width;
	int height = frame->height;
	int nChannels = frame->nChannels;
	//cluster for rectangle
	int cluster_dist_threshold = 15;
	int rect_threshold = 10;

	output_data = (unsigned char *) malloc (width * height * nChannels);
    IplImage *foreground = cvCreateImage(cvGetSize(frame), frame->depth, 1);
	foreground->imageData = (char *) output_data;

	cvNamedWindow("FG",1);
	cvNamedWindow("ORI",1);
	while (frame)
	{
		image_data = (unsigned char *) frame->imageData;
		if (!vbM)
			vbM = VibeModelInit(width, height, nChannels, image_data);
		VibeModelUpdate(vbM, image_data, output_data);
		//open and close operation
        cvMorphologyEx(foreground, foreground, 0, 0, CV_MOP_CLOSE, 1);
        cvMorphologyEx(foreground, foreground, 0, 0, CV_MOP_OPEN, 1);

        // cluster to get large region of foreground pixel
        bs_bbox.clear();

        find_rect_region(output_data, width, height, 0, 
            cluster_dist_threshold, rect_threshold, bs_bbox);
        cout << bs_bbox.size() << endl;
		 //draw bs_bbox in the foreground image
		cv::Rect r;
        for (int i = 0; i < bs_bbox.size(); i++) {
            r = bs_bbox[i];
            //r = bs_region_bbox[i];
            //cvRectangle(currentImg, r.tl(), r.br(), cv::Scalar(255,0,0), 2); // draw bs rectangle in whole img
            cvRectangle(foreground, r.tl(), r.br(), cv::Scalar(255), 2); // draw bs rectangle in foreground img
        }
		cvShowImage("ORI", frame);
		cvShowImage("FG", foreground);

		if(cvWaitKey( 1 ) == 27) break;
		
		frame = cvQueryFrame(video);
	}	
	

	cvReleaseCapture(&video);
	cvDestroyWindow("FG");
	cvDestroyWindow("ORI");
	system("pause");
	return 0;
}