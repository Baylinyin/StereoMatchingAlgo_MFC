#include "StdAfx.h"
#include "Census.h"


Census::Census(void) { }
Census::~Census(void) { }


void Census::run(unsigned char *leftImgData, unsigned char *rightImgData, unsigned char *disparityImgData,
					int imgHeight, int imgWidth, int disparityMin, int disparityMax, int lrCheckThr, int uniquenessThr, int corrWinSize, int paramWinSize) {
	int height = imgHeight;					int width = imgWidth;
	int disparity_min = disparityMin;		int disparity_max = disparityMax;
	int lrcheck = lrCheckThr;				float uniqueT = float(uniquenessThr)/100;
	int winsize = corrWinSize;				int hsize = winsize/2;								int npwsize = paramWinSize;
	
	int shift = disparity_max - disparity_min;
	int buffer = width*shift;
	int census_depth = (npwsize * npwsize - 1)/ 8;	
	int hnpsize = npwsize/2;
	int multi = 256/shift;

	int sizeOfXMMVector = 16;

	int ynpmax = height - hnpsize;
	int xnpmax1 = width - hnpsize - sizeOfXMMVector;	int xnpmax2 = width - hnpsize ;
	int xmax1 = width - hsize - sizeOfXMMVector;		int xmax =  width - /*shift  <== NTT */ - hsize ;				int xmax2 = xmax - sizeOfXMMVector;
	int ymax =  height - hsize ;
	int censusbuf = width*census_depth;
	int widthByHnpsize = hnpsize*width;

	int row, col;		int x,y;		int d;		int counter;

	unsigned char* census_l;
	unsigned char* census_r;	
	unsigned char* xbuffer_r1;
	unsigned char* xbuffer_l1;
	unsigned char* xbuffer_r;
	unsigned char* xbuffer_l;
	unsigned char *img_r1, *img_r2, *img_r3, *img_r4;
	unsigned char *img_l1, *img_l2, *img_l3, *img_l4;
	unsigned short * dis3D;		
	unsigned short *xbuffer, *xbuffer1, *ybuffer, *ybuffer1;
	
	int max_size;
	
	unsigned char distance_table[256]= {
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
	};

	max_size = height*width*census_depth;	census_l = new unsigned char [max_size];			census_r = new unsigned char [max_size];		
	memset(disparityImgData,0,width*height);			memset(census_l,0,max_size*sizeof(unsigned char));	memset(census_r,0,max_size*sizeof(unsigned char));
	max_size = height*width*shift;			dis3D = new unsigned short [ max_size ];			memset(dis3D,0,max_size*sizeof(unsigned short));

	unsigned char xmmMask[16] = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01};


	// ------------------- begin transform -------------------
	counter = 0;

	xbuffer_r1 = &census_r[hnpsize*censusbuf];
	xbuffer_l1 = &census_l[hnpsize*censusbuf];

	img_r1 = &rightImgData[hnpsize*width];
	img_l1 = &leftImgData[hnpsize*width];

	for(y=hnpsize; y < ynpmax; y++)	{
		img_r2 = img_r1 + hnpsize;
		img_l2 = img_l1 + hnpsize;
		xbuffer_r = &xbuffer_r1[hnpsize];
		xbuffer_l = &xbuffer_l1[hnpsize];
		
		for(x=hnpsize; x <= xnpmax2; x++) {			
			img_r3 = img_r2 - widthByHnpsize;
			img_l3 = img_l2 - widthByHnpsize;

			for(row = -hnpsize; row <= hnpsize; row++) {
				img_r4 = img_r3 - hnpsize;
				img_l4 = img_l3 - hnpsize;

				for(col = -hnpsize; col <= hnpsize; col++) {
					if(col == 0 && row == 0 ) {
						img_r4++;
						img_l4++;	
						continue;
					}
										
					int temp;
					temp = *img_r2 - *img_r4;				*xbuffer_r = (*xbuffer_r) << 1;					
					if( temp > 0 ) {						*xbuffer_r = (*xbuffer_r) | 0x01;					}
		
					temp = *img_l2 - *img_l4;				*xbuffer_l = (*xbuffer_l) << 1;
					if( temp > 0 ) {						*xbuffer_l  = (*xbuffer_l) | 0x01;					}
									
					if(counter == 7) {
						xbuffer_r += width;
						xbuffer_l += width;
						counter = 0;
					} else {
						counter++;
					}
					img_r4++;
					img_l4++;	
				}
				img_r3 += width;
				img_l3 += width;
			}			
			img_r2 ++;
			img_l2 ++;

			xbuffer_r++;		xbuffer_r -= censusbuf;
			xbuffer_l++;		xbuffer_l -= censusbuf;
		}	
		img_r1 += width;
		img_l1 += width;
		xbuffer_r1 += censusbuf;
		xbuffer_l1 += censusbuf;
	}

	// ----------------- hamming distance -----------------
	int hamxmax = width  /*- shift  <== NTT */;

	BYTE* bufyl = census_l;
	BYTE* bufyr = census_r;

	unsigned short* bufy3d = dis3D;
	
	for(y = 0; y < height; y++)	{
		unsigned short* bufx3d = &bufy3d[-disparity_min];

		for(x = 0; x < hamxmax; x++) {
			for(d = disparity_min; d < disparity_max; d++) {
				int sum = 0;				
				BYTE* bufnl = &bufyl[x];
				BYTE* bufnr = &bufyr[x];

				for(int n = 0; n < census_depth; n++) {
					unsigned char bufferl = bufnl[d];
					unsigned char bufferr = bufnr[0];
					sum += distance_table[ bufferl^bufferr ];
					bufnl += width;
					bufnr += width;
				}
				bufx3d[d] = sum;				
			}
			bufx3d += shift;
		}
		bufyl += censusbuf;
		bufyr += censusbuf;
		bufy3d += buffer;
	}

	// -----------
	y = hsize+1;
	x = hsize+1;

	ybuffer1 = dis3D;	
	for( d = 0 ; d < shift ; d++ ) { 
		unsigned short sum2 = 0;
		ybuffer = &dis3D[buffer+d]; 
		for (row = -hsize ; row <= hsize ; row++) {
			xbuffer = &ybuffer[shift];
			unsigned short sum1=0;
			for (col = -hsize ; col <= hsize ; col++) {			
				sum1 += *xbuffer;
				xbuffer += shift;
			}
			*ybuffer = sum1;
			sum2 += sum1;
			ybuffer += buffer;
		}
		*(ybuffer1++) = sum2;
	}

	for(x=hsize+2; x < xmax; x++) {
		ybuffer1 = &dis3D[(y-(hsize+1))*buffer +(x-(hsize+1))*shift];		
		for( d = 0 ; d < shift ; d++ ) { 
			unsigned short sum1=0;
			ybuffer = &dis3D[(y-hsize)*buffer+d]; 
			for (row = -hsize ; row <= hsize; row++) {			
				ybuffer[(x-(hsize+1))*shift] = ybuffer[(x-(hsize+2))*shift] 
											 - ybuffer[(x-(hsize+1))*shift]
											 + ybuffer[(x+hsize)*shift];
				sum1 += ybuffer[(x-(hsize+1))*shift];	
				ybuffer += buffer;
			}
			*(ybuffer1++) = sum1;
		}
	}

	for(y=hsize+2; y < ymax; y++) {
		x = hsize+1;
		ybuffer1 = &dis3D[(y-(hsize+1))*buffer +(x-(hsize+1))*shift];
		xbuffer1 = &dis3D[(y+(hsize))*buffer];
		xbuffer = &dis3D[(y-(hsize+2))*buffer + (x-(hsize+1))*shift];
		
		for( d = 0 ; d < shift ; d++ ) { 
			unsigned short sum1=0;
			ybuffer = &xbuffer1[(x-hsize)*shift + d];

			for (col = -hsize ; col <= hsize; col++) {				
				sum1 += xbuffer1[(x+col)*shift + d];
			}
			ybuffer[-shift]  = sum1 ;			
			ybuffer1[d] = sum1 + xbuffer[d] - ybuffer1[d];
		}

		// --------
		xbuffer1 = &dis3D[(y+hsize)*buffer];

		for(x=hsize+2; x < xmax; x++) { 
			ybuffer = &dis3D[(y-(hsize+2))*buffer + (x-(hsize+1))*shift];
			ybuffer1= &xbuffer1[(x+hsize)*shift];
			xbuffer = &xbuffer1[(x-(hsize+2))*shift];
			
			for( d = 0 ; d < shift ; d++ ) { 
				xbuffer[shift] = *xbuffer - xbuffer[shift] + *ybuffer1;
				ybuffer[buffer] = *ybuffer - ybuffer[buffer] + xbuffer[shift];
				ybuffer++;
				xbuffer++;
				ybuffer1++;
			}
		}
	}

	// -------------------- make initial disparity map --------------------
	unsigned short temp1, temp2;
	int n;
	for (y = hsize + 1; y < ymax-1 ; y++ ) {
		for ( x = hsize  /*+shift  <== NTT */ + 1 ; x < xmax-1 ; x++  ) {
			n = 0 ; 
			temp1 = USHRT_MAX;

			for(d = 0; d < shift ; d++)	{
				temp2 = dis3D[(y-(hsize+1))*width*shift + (x-(hsize+1))*shift + d] ;
				if ( temp1 > temp2 ) {
					temp1 = temp2 ;
					n = d ;
				 }
			}

			// LR check
			int d1 =0 ;
			temp1 = USHRT_MAX;
			unsigned short unique1=USHRT_MAX;
			unsigned short unique2=USHRT_MAX;

			for (d=0 ; d < shift ; d++) {
				temp2 = dis3D[(y-(hsize+1))*width*shift + (x-(hsize+1)+(n-d))*shift + d ] ;
				if ( temp1 > temp2 ) {
					unique2 = unique1;
					unique1 = temp1;
					temp1 = temp2 ;
					d1 = d ;
				 }
			}

			if ( (abs(d1-n) <= lrcheck) && ((unique2-temp1) >= (temp1*uniqueT))) {
				float temp22 = -(float)dis3D[(y-(hsize+1))*width*shift + (x-(hsize+1))*shift + n-1] ;
				float temp3 =  -(float)dis3D[(y-(hsize+1))*width*shift + (x-(hsize+1))*shift + n+1] ;
				float temp4 =2*(float)(-temp1);
				temp4 = (temp22-temp3)/(temp22-temp4+temp3);
				temp4 *= 0.5;
				
				if(1 < fabs(temp4)) { temp4 = 0.0; }
				float zk = n + temp4;
				disparityImgData[y*width+x] = n*multi ;
			} else {
				disparityImgData[y*width +x] = 0 ;
			}
		}
	}
	delete [] dis3D;
	delete [] census_l;
	delete [] census_r;
}