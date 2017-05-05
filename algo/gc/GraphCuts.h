#pragma once

#include "cv.h"
#include "highgui.h"


class GraphCuts
{
public:
	GraphCuts(void);
	~GraphCuts(void);

	void GC_OpenCV(IplImage *leftImg, IplImage *rightImg, IplImage *disparityImg, IplImage* tmpImg,
					int Ithreshold, int interactionRadius, int occlusionCost, 
					int minDisparity, int numberOfDisparities, int maxIters);
};

