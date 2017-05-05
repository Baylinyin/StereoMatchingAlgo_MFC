#pragma once


class Census
{

public:
	Census(void);
	~Census(void);

	void run(unsigned char *leftImgData, unsigned char *rightImgData, unsigned char *disparityImgData,
			int imgHeight, int imgWidth, int disparityMin, int disparityMax, int lrCheckThr, int uniquenessThr, int corrWinSize, int paramWinSize);
};

