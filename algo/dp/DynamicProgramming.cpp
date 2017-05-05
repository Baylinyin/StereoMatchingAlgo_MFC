#include "StdAfx.h"
#include "DynamicProgramming.h"


DynamicProgramming::DynamicProgramming(void) { }
DynamicProgramming::~DynamicProgramming(void) { }


void DynamicProgramming::run(IplImage *leftImg, IplImage *rightImg, IplImage *disparityImg,
							int occlusionVal) {
	int Height = leftImg->height;
	int Width = leftImg->width;
	int i = 0, j = 0, k = 0, P = 0, Q = 0;
	int *scanLine_Left, *scanLine_Right;
	int **PreMap, **disparityMap, **result;
	int min1, min2, min3; 
	int val, disparityValue, BeforeState;

	// --- init ---
	scanLine_Left = (int*) malloc(sizeof(int)*Width);		scanLine_Right = (int*) malloc(sizeof(int)*Width);
	PreMap = (int**) malloc(sizeof(int*)*Width);			disparityMap = (int**) malloc(sizeof(int*)*Width);
	result = (int**) malloc(sizeof(int*)*Height);

	for(i=0;i<Height;i++)		result[i] = (int *) malloc(sizeof(int)*Width); 
	for(i=0;i<Width;i++) {
		PreMap[i] = (int *) malloc(sizeof(int)*Width);
		disparityMap[i] = (int *) malloc(sizeof(int)*Width);
	}

	for(j=0;j<Height;j++) 		for(k=0;k<Width;k++)	result[j][k] = 0;
	for(j=0;j<Width;j++) {
		scanLine_Left[j] = 0;
		scanLine_Right[j] = 0;
		for(k=0;k<Width;k++) {	PreMap[j][k] = 0;		disparityMap[j][k] = 0;		}
	}

	// --- matching every pair of scanLine ---
	for(i=0;i<Height;i++) {
		
		// scanLine
		for(j=0;j<Width;j++) {
			scanLine_Left[j] = leftImg->imageData[(i*leftImg->widthStep)+j];
			scanLine_Right[j] = rightImg->imageData[(i*rightImg->widthStep)+j];
		}

		// disparityMap scanLine data
		for(j=0;j<Width;j++) {
			disparityMap[j][0] = scanLine_Left[j];
			disparityMap[0][j] = scanLine_Right[j];
		}

		// store all "path" of matching (like Djikstra algorithm in finding shortest path)
		for(k = 1; k < Width; k++) {
			for(j = 1; j < Width; j++) {

				// calculate the difference btw two pixels in scanLine_left and scanLine_right
				if(disparityMap[j][0] < 0)	disparityMap[j][0] *= -1; // take the absolute
				if(disparityMap[0][k] < 0)	disparityMap[0][k] *= -1; // take the absolute
				val = disparityMap[j][0] - disparityMap[0][k]; 
				if(val < 0)		val = val* -1; // take the absolute				
				
				// store the best path (min difference) (thus, the difference and occlusion value are accumulated to disparityMap)
				min1 = disparityMap[j-1][k-1] + val;		// top-left value + difference	 --> update the difference for previous matching
				min2 = disparityMap[j-1][k] + occlusionVal; // top value	  + occlusionVal --> left occluded
				min3 = disparityMap[j][k-1] + occlusionVal; // left value	  + occlusionVal --> right occluded
				
				// the best current path (min(m1, m2, m3)) is chosen
				if(min1 <= min2 && min1 <= min3) {	PreMap[j][k] = 1;	disparityMap[j][k] = min1;	} // min(m1, m2, m3) = m1
				if(min2 <= min1 && min2 <= min3) {	PreMap[j][k] = 2;	disparityMap[j][k] = min2;	} // min(m1, m2, m3) = m2
				if(min3 <= min1 && min3 <= min2) {	PreMap[j][k] = 3;	disparityMap[j][k] = min3;	} // min(m1, m2, m3) = m3
			}			
		}

		//
		P = Width-1;
		Q = Width-1;
		BeforeState = 1;
		disparityValue = 0;

		while(Q!=0 && P != 0) {
			switch(PreMap[P][Q]) {
				case 1: // no occlusion (sequential)
					result[i][Q] = disparityValue;
					if(BeforeState == 3 || BeforeState == 2) {
						val = P-Q;
						if(val<0) val = val * -1;
						disparityValue = val; // disparityValue = abs(columnOfLeft - columnOfRight)
					}
					BeforeState = 1;
					P--;
					Q--;
					break;
				case 2: // left occluded
					result[i][Q] = disparityValue;		BeforeState = 2;		P--;		break;
				case 3: // right occluded
					result[i][Q] = disparityValue;		BeforeState = 3;		Q--;		break;
			}
		}
	}
	for(i=0;i<Height;i++) 	for(j=0;j<Width;j++) 	disparityImg->imageData[i*disparityImg->widthStep+j] = (result[i][j])*(16);			

	// release
	for(i=0;i<Height;i++) {	free(PreMap[i]);	free(disparityMap[i]);		free(result[i]);	}
	free(scanLine_Left);	free(scanLine_Right);
	free(PreMap);			free(disparityMap);
	free(result);
}