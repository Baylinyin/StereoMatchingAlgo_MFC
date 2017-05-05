#pragma once

#include "cv.h"
#include "highgui.h"


class DynamicProgramming
{
public:
	DynamicProgramming(void);
	~DynamicProgramming(void);

	void run(IplImage *leftImg, IplImage *rightImg, IplImage *disparityImg, int occlusionVal);
};