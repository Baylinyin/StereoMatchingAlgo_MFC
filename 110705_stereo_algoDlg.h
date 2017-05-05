#pragma once

#include "cv.h"
#include "highgui.h"

#include "util\MyTimer.h"
#include "util\Util.h"

#include "algo\StereoAlgo.h"


// ---------------------------------------------

#define WIN_LEFT_IMG			"Left image"
#define WIN_RIGHT_IMG			"Right image"
#define WIN_DISPARITY_IMG		"Disparity image"
#define WIN_TMP_IMG				"Tmp image"


#define CENSUS_MAX_SEARCH_DISPARITY					64
#define CENSUS_MIN_SEARCH_DISPARITY					-32		// -16
#define CENSUS_CORRELATION_WIN_SIZE					11
#define CENSUS_PARAMETER_WIN_SIZE					7
#define CENSUS_LR_CHECK_THR							1
#define CENSUS_UNIQUENESS_TEST						0

#define SAD_MAX_SEARCH_DISPARITY					64
#define SAD_MIN_SEARCH_DISPARITY					-32		// 0
#define SAD_CORRELATION_WIN_SIZE					11
#define SAD_LR_CHECK_THR							1
#define SAD_UNIQUENESS_TEST							0

#define DYNAMIC_PROGRAMMING_OCCLUSION_VALUE			30

#define SAD_OPENCV_PRE_FILTER_SIZE					41
#define SAD_OPENCV_PRE_FILTER_CAP					15		// 31
#define SAD_OPENCV_SAD_WINDOW_SIZE					15		// 41
#define SAD_OPENCV_MIN_DISPARITY					-32		// -64
#define SAD_OPENCV_NUMBER_OF_DISPARITIES			96		// 128
#define SAD_OPENCV_TEXTURE_THRESHOLD				10
#define SAD_OPENCV_UNIQUENESS_RATIO					15
#define SAD_OPENCV_SPECKLE_WINDOW_SIZE				0		// 0: disable
#define SAD_OPENCV_SPECKLE_RANGE					0
#define SAD_OPENCV_DISP12MAXDIFF					-1		// -1: disable

#define GRAPH_CUTS_OPENCV_ITHRESHOLD				5
#define GRAPH_CUTS_OPENCV_INTERACTION_RADIUS		1
#define GRAPH_CUTS_OPENCV_OCCLUSION_COST			10000
#define GRAPH_CUTS_OPENCV_MIN_DISPARITY				-32
#define GRAPH_CUTS_OPENCV_NUMBER_OF_DISPARITIES		96
#define GRAPH_CUTS_OPENCV_MAX_ITERS					2


// ---------------------------------------------


class CMy110705_stereo_algoDlg : public CDialogEx
{
// Construction
public:
	CMy110705_stereo_algoDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MY110705_STEREO_ALGO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


public:	
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonLoadImgSeq();

	afx_msg void OnBnClickedButtonCensus();
	afx_msg void OnBnClickedButtonSad();	
	afx_msg void OnBnClickedButtonDynamicProgramming();	
	afx_msg void OnBnClickedButtonSadOpencv();
	afx_msg void OnBnClickedButtonGraphCutsOpencv();


	void initVar();
	void initGUI();
	void getInputParam();
	void displayResult();


private:

	// ----- input parameters -----
	int censusMaxSearchDisparity, censusMinSearchDisparity, censusCorrelationWinSize, censusParameterWinSize, censusLRcheckThr, censusUniquenessTest;
	int sadMaxSearchDisparity, sadMinSearchDisparity, sadCorrelationWinSize, sadLRcheckThr, sadUniquenessTest;
	int dynamicProgrammingOcclusionValue;
	int sadOpenCVpreFilterSize, sadOpenCVpreFilterCap, sadOpenCVSADWindowSize, sadOpenCVminDisparity, sadOpenCVnumberOfDisparities, sadOpenCVtextureThreshold, sadOpenCVuniquenessRatio, sadOpenCVspeckleWindowSize, sadOpenCVspeckleRange, sadOpenCVdisp12MaxDiff;
	int graphCutsIthreshold, graphCutsInteractionRadius, graphCutsOcclusionCost, graphCutsMinDisparity, graphCutsNumberOfDisparities, graphCutsMaxIters;


	// ----- util -----
	char charArray[256];
	bool isLoadImg;
	int processingTime;	

	CString strFilename;
	int imgFromIdx, imgToIdx;
	CString leftImgSeqPath, rightImgSeqPath, disparityImgSeqPath, tmpImgSeqPath;


	// ----- images -----
	IplImage *leftImg, *rightImg, *disparityImg, *tmpImg;


	// ----- other classes -----
	MyTimer myTimer;
	StereoAlgo stereoAlgo;
};
