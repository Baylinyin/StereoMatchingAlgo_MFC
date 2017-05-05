#pragma once

#include "cv.h"
#include "highgui.h"

#include "..\util\Util.h"

#include "census\Census.h"
#include "sad\SAD.h"
#include "dp\DynamicProgramming.h"
#include "gc\GraphCuts.h"


class StereoAlgo
{
public:
	StereoAlgo(void);
	~StereoAlgo(void);

	void census(IplImage* leftImg, IplImage* rightImg, IplImage* disparityImg,
				int censusCorrelationWinSize, 
				int censusMaxSearchDisparity, int censusMinSearchDisparity, 
				int censusLRcheckThr, int censusUniquenessTest, 
				int censusParameterWinSize);

	void sad(IplImage* leftImg, IplImage* rightImg, IplImage* disparityImg,
				int sadCorrelationWinSize, 
				int sadMaxSearchDisparity, int sadMinSearchDisparity, 
				int sadLRcheckThr, int sadUniquenessTest);

	void dynamicProgramming(IplImage* leftImg, IplImage* rightImg, IplImage* disparityImg,
				int occlusionValue);

	void sadOpenCV(IplImage* leftImg, IplImage* rightImg, IplImage* disparityImg,
				int preFilterSize, int preFilterCap,
				int SADWindowSize, int minDisparity, int numberOfDisparities,
				int textureThreshold, int uniquenessRatio, int speckleWindowSize, int speckleRange, int disp12MaxDiff);

	void graphCutsOpenCV(IplImage* leftImg, IplImage* rightImg, IplImage* disparityImg, IplImage* tmpImg,
				int Ithreshold, int interactionRadius, int occlusionCost, 
				int minDisparity, int numberOfDisparities, int maxIters);


private:
	Census censusAlgo;
	SAD sadAlgo;
	DynamicProgramming dynamicProgrammingAlgo;
	GraphCuts graphCutsAlgo;
};
