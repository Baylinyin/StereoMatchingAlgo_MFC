#include "StdAfx.h"
#include "GraphCuts.h"


GraphCuts::GraphCuts(void) { }
GraphCuts::~GraphCuts(void) { }


void GraphCuts::GC_OpenCV(IplImage *leftImg, IplImage *rightImg, IplImage *disparityImg, IplImage* tmpImg,
							int Ithreshold, int interactionRadius, int occlusionCost, 
							int minDisparity, int numberOfDisparities, int maxIters) {

	// ----- set parameters -----
	CvStereoGCState *GCState = cvCreateStereoGCState(numberOfDisparities, maxIters);
	
	GCState->Ithreshold				= Ithreshold;			// threshold for piece-wise linear data cost function (5 by default)
	GCState->interactionRadius		= interactionRadius;	// radius for smoothness cost function (1 by default; means Potts model)
    //float K, lambda, lambda1, lambda2;					// parameters for the cost function (usually computed adaptively from the input data)
    GCState->occlusionCost			= occlusionCost;		// 10000 by default
    GCState->minDisparity			= minDisparity;			// 0 by default
    //GCState->numberOfDisparities	= numberOfDisparities; 
    //GCState->maxIters				= maxIters;				// number of iterations; defined by user.
	

	// ----- matching -----
	IplImage *dispLeftImg = cvCreateImage(cvSize(leftImg->width, leftImg->height), IPL_DEPTH_16S, 1); 
	IplImage *dispRightImg = cvCreateImage(cvSize(leftImg->width, leftImg->height), IPL_DEPTH_16S, 1); 

	cvFindStereoCorrespondenceGC(leftImg, rightImg, dispLeftImg, dispRightImg, GCState);	
	cvConvertScale(dispLeftImg, disparityImg, -16);
	cvConvertScale(dispRightImg, tmpImg, -16);

	cvReleaseImage(&dispLeftImg);
	cvReleaseImage(&dispLeftImg);
	cvReleaseStereoGCState(&GCState);
}
