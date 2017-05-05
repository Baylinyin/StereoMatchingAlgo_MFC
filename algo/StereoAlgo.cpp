#include "StdAfx.h"
#include "StereoAlgo.h"


StereoAlgo::StereoAlgo(void) { }
StereoAlgo::~StereoAlgo(void) { }


// =====================================================================================


void StereoAlgo::census(IplImage* leftImg, IplImage* rightImg, IplImage* disparityImg,
						int censusCorrelationWinSize, 
						int censusMaxSearchDisparity, int censusMinSearchDisparity, 
						int censusLRcheckThr, int censusUniquenessTest, 
						int censusParameterWinSize) {
	unsigned char *leftImgData = (unsigned char *) leftImg->imageData;
	unsigned char *rightImgData = (unsigned char *) rightImg->imageData;	
	unsigned char *disparityImgData = (unsigned char *) disparityImg->imageData;
	censusAlgo.run(leftImgData, rightImgData, disparityImgData,
					leftImg->height, leftImg->width, censusMinSearchDisparity, censusMaxSearchDisparity, censusLRcheckThr, censusUniquenessTest, censusCorrelationWinSize, censusParameterWinSize);
}


void StereoAlgo::sad(IplImage* leftImg, IplImage* rightImg, IplImage* disparityImg,
						int sadCorrelationWinSize, 
						int sadMaxSearchDisparity, int sadMinSearchDisparity, 
						int sadLRcheckThr, int sadUniquenessTest) {
	unsigned char *leftImgData = (unsigned char *) leftImg->imageData;
	unsigned char *rightImgData = (unsigned char *) rightImg->imageData;	
	unsigned char *disparityImgData = (unsigned char *) disparityImg->imageData;
	sadAlgo.run(leftImgData, rightImgData, disparityImgData,
				leftImg->height, leftImg->width, sadMinSearchDisparity, sadMaxSearchDisparity, sadLRcheckThr, sadUniquenessTest, sadCorrelationWinSize);
}


void StereoAlgo::dynamicProgramming(IplImage* leftImg, IplImage* rightImg, IplImage* disparityImg,
						int occlusionValue) {
	dynamicProgrammingAlgo.run(leftImg, rightImg, disparityImg, occlusionValue);
}


void StereoAlgo::sadOpenCV(IplImage* leftImg, IplImage* rightImg, IplImage* disparityImg,
						int preFilterSize, int preFilterCap,
						int SADWindowSize, int minDisparity, int numberOfDisparities,
						int textureThreshold, int uniquenessRatio, int speckleWindowSize, int speckleRange, int disp12MaxDiff) {
	sadAlgo.SAD_OpenCV(leftImg, rightImg, disparityImg,
					preFilterSize, preFilterCap,
					SADWindowSize, minDisparity, numberOfDisparities,
					textureThreshold, uniquenessRatio, speckleWindowSize, speckleRange, disp12MaxDiff);
}


void StereoAlgo::graphCutsOpenCV(IplImage* leftImg, IplImage* rightImg, IplImage* disparityImg, IplImage* tmpImg,
						int Ithreshold, int interactionRadius, int occlusionCost, 
						int minDisparity, int numberOfDisparities, int maxIters) {
	graphCutsAlgo.GC_OpenCV(leftImg, rightImg, disparityImg, tmpImg,
							Ithreshold, interactionRadius, occlusionCost, 
							minDisparity, numberOfDisparities, maxIters);
}