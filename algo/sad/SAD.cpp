#include "StdAfx.h"
#include "SAD.h"


SAD::SAD(void) { }
SAD::~SAD(void) { }


// =========================================================================================


void SAD::run(unsigned char *leftImgData, unsigned char *rightImgData, unsigned char *disparityImgData,
				int imgHeight, int imgWidth, int disparityMin, int disparityMax, int lrCheckThr, int uniquenessThr, int corrWinSize) {
	int height = imgHeight;							int width = imgWidth;
	int disparity_min = disparityMin;				int disparity_max = disparityMax;
	int lrcheck = lrCheckThr;						float uniqueT = (float)(uniquenessThr)/100;
	int winsize = corrWinSize;						int hsize = winsize/2;
	int shift = disparity_max - disparity_min ; 
	int buffer = width*shift;
	int max_size = height*width*shift;

	int multi = 256/shift;
	int row, col;				int x,y;			int d;
	unsigned short * dis3D; 	dis3D = new unsigned short[max_size];
	int xmax, ymax;
	ymax =  height - hsize;
	xmax =  width - hsize;

	// matching
	for (y=hsize+1 ; y < ymax ; y++) {	
		for (x=hsize+1 ; x < xmax ; x++) {
			for (d = disparity_min ; d < disparity_max ; d++) {
				unsigned short sum = 0;
				for (row = -hsize ; row <= hsize ; row++) {
					unsigned short sum1=0;
					for (col = -hsize ; col <= hsize ; col++) 
						sum += abs ( leftImgData[(y+row)*width + x+d+col] - rightImgData[(y+row)*width+x+col]) ;
				}
				dis3D[y*buffer + x*shift + d-disparity_min] = sum;
			}
		}
	}

	// make initial disparity map
	unsigned short temp1, temp2;
	int n;
	for (y = hsize+1 ; y < ymax-1 ; y++ ) {
		for ( x = hsize + 1 ; x < xmax-1 ; x++ ) {
			n = 0 ; 
			temp1 = USHRT_MAX ;
			for(d = 1; d < shift-1 ; d++) {
				temp2 = dis3D[y*buffer + x*shift + d] ;
				if ( temp1 > temp2 ) {
					temp1 = temp2 ;
					n = d ;
				 }
			}
			disparityImgData[y*width+x] = n*multi;
			
			// LR check
			int d1 =0 ;
			temp1 = USHRT_MAX;
			unsigned short unique1 = USHRT_MAX;
			unsigned short unique2 = USHRT_MAX;

			for(d = 0; d < shift; d++) 			{
				temp2 = dis3D[y*buffer + (x+(n-d))*shift + d ] ;
				if ( temp1 > temp2 ) {
					unique2 = unique1;
					unique1 = temp1;
					temp1 = temp2 ;
					d1 = d ;
				 }
			}
			
			if ( (abs(d1-n) <= lrcheck) && ((unique2-temp1) >= (temp1*uniqueT))) {
				disparityImgData[y*width+x] = n*multi ;
			} else {
				disparityImgData[y*width+x] = 0 ;
			}
		}
	}
	delete [] dis3D;
}


// =========================================================================================


void SAD::SAD_OpenCV(IplImage *leftImg, IplImage *rightImg, IplImage *disparityImg,
					int preFilterSize, int preFilterCap,
					int SADWindowSize, int minDisparity, int numberOfDisparities,
					int textureThreshold, int uniquenessRatio, int speckleWindowSize, int speckleRange, int disp12MaxDiff) {

	// ----- set parameters -----
	CvStereoBMState *BMState = cvCreateStereoBMState();
	
	// pre filters (normalize input images)
	BMState->preFilterSize			= preFilterSize;
    BMState->preFilterCap			= preFilterCap;			// up to 31

	// correspondence using Sum of Absolute Difference
    BMState->SADWindowSize			= SADWindowSize;
    BMState->minDisparity			= minDisparity;
    BMState->numberOfDisparities	= numberOfDisparities;	// = maximum disparity - minimum disparity

	// post filters (knock out bad matches)
    BMState->textureThreshold		= textureThreshold;		// areas with no texture are ignored
    BMState->uniquenessRatio		= uniquenessRatio;		// invalidate disparity at pixels where there are other close matches with different disparity
	BMState->speckleWindowSize		= speckleWindowSize;	// the maximum area of speckles to remove (set to 0 to disable speckle filtering)
	BMState->speckleRange			= speckleRange;			// acceptable range of disparity variation in each connected component
	BMState->disp12MaxDiff			= disp12MaxDiff;		// maximum allowed disparity difference in the left-right check


	// ----- matching -----
	IplImage *preDisparityImg = cvCreateImage(cvSize(leftImg->width, leftImg->height), IPL_DEPTH_16S, 1); 

	cvFindStereoCorrespondenceBM(leftImg, rightImg, preDisparityImg, BMState);
	cvNormalize(preDisparityImg, disparityImg, 0, 256, CV_MINMAX);

	cvReleaseImage(&preDisparityImg);
	cvReleaseStereoBMState(&BMState);
}
