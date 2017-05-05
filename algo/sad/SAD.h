#pragma once

#include "cv.h"
#include "highgui.h"


class SAD
{

public:
	SAD(void);
	~SAD(void);

	void run(unsigned char *leftImgData, unsigned char *rightImgData, unsigned char *disparityImgData,
			int imgHeight, int imgWidth, int disparityMin, int disparityMax, int lrCheckThr, int uniquenessThr, int corrWinSize);

	void SAD_OpenCV(IplImage *leftImg, IplImage *rightImg, IplImage *disparityImg,
					int preFilterSize, int preFilterCap,
					int SADWindowSize, int minDisparity, int numberOfDisparities,
					int textureThreshold, int uniquenessRatio, int speckleWindowSize, int speckleRange, int disp12MaxDiff);
};

