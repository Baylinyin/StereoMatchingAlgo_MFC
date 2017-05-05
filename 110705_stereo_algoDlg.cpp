#include "stdafx.h"
#include "110705_stereo_algo.h"
#include "110705_stereo_algoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMy110705_stereo_algoDlg::CMy110705_stereo_algoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMy110705_stereo_algoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy110705_stereo_algoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMy110705_stereo_algoDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON_LOAD_IMG_SEQ, &CMy110705_stereo_algoDlg::OnBnClickedButtonLoadImgSeq)
	ON_BN_CLICKED(IDC_BUTTON_CENSUS, &CMy110705_stereo_algoDlg::OnBnClickedButtonCensus)
	ON_BN_CLICKED(IDC_BUTTON_SAD, &CMy110705_stereo_algoDlg::OnBnClickedButtonSad)
	ON_BN_CLICKED(IDC_BUTTON_DYNAMIC_PROGRAMMING, &CMy110705_stereo_algoDlg::OnBnClickedButtonDynamicProgramming)
	ON_BN_CLICKED(IDC_BUTTON_SAD_OPENCV, &CMy110705_stereo_algoDlg::OnBnClickedButtonSadOpencv)
	ON_BN_CLICKED(IDC_BUTTON_GRAPH_CUTS_OPENCV, &CMy110705_stereo_algoDlg::OnBnClickedButtonGraphCutsOpencv)	
END_MESSAGE_MAP()


// CMy110705_stereo_algoDlg message handlers

BOOL CMy110705_stereo_algoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	initVar();
	initGUI();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CMy110705_stereo_algoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMy110705_stereo_algoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// =====================================================================================


void CMy110705_stereo_algoDlg::OnClose() {
	cvDestroyAllWindows();	
	if(isLoadImg) {
		cvReleaseImage(&leftImg);
		cvReleaseImage(&rightImg);
		cvReleaseImage(&disparityImg);
		// not yet release tmpImg
	}
	CDialogEx::OnClose();
}


void CMy110705_stereo_algoDlg::OnBnClickedButtonLoadImgSeq() {
	CFileDialog dlg(TRUE, _T("*.txt"), _T(""), OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST|OFN_HIDEREADONLY, _T("Image list (*.txt)|*.txt|All Files (*.*)|*.*||"), NULL);

	if (dlg.DoModal() == IDOK) {		
		CStdioFile imgSeqFile;
		imgSeqFile.Open((LPCTSTR)dlg.GetPathName(), CFile::modeRead);

		CString text;
		imgSeqFile.ReadString(text);	imgFromIdx = int(wcstod(text, NULL));
		imgSeqFile.ReadString(text);	imgToIdx = int(wcstod(text, NULL));

		imgSeqFile.ReadString(leftImgSeqPath);
		imgSeqFile.ReadString(rightImgSeqPath);
		imgSeqFile.ReadString(disparityImgSeqPath);
		imgSeqFile.ReadString(tmpImgSeqPath);

		imgSeqFile.Close();
	}

	// ----- input images were loaded, init parameters -----
	isLoadImg = true;
	//disparityImg = cvCreateImage(cvSize(640, 480), IPL_DEPTH_8U, 1); // init disparity image
	//cvZero(disparityImg);

	// ----- input images were loaded, activate GUI -----	
	this->GetDlgItem(IDC_BUTTON_CENSUS)->EnableWindow(true);
	this->GetDlgItem(IDC_BUTTON_SAD)->EnableWindow(true);
	this->GetDlgItem(IDC_BUTTON_DYNAMIC_PROGRAMMING)->EnableWindow(true);
	this->GetDlgItem(IDC_BUTTON_SAD_OPENCV)->EnableWindow(true);
	this->GetDlgItem(IDC_BUTTON_GRAPH_CUTS_OPENCV)->EnableWindow(true);
}


// --------------------------------------


void CMy110705_stereo_algoDlg::OnBnClickedButtonCensus() {
	getInputParam();	

	for(int i = imgFromIdx; i <= imgToIdx; i++) {

		// ----- load the left and right images
		strFilename.Format(leftImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		leftImg = cvLoadImage(charArray, 0);
		strFilename.Format(rightImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		rightImg = cvLoadImage(charArray, 0);
		
		// ----- init the disparity image
		disparityImg = cvCreateImage(cvSize(leftImg->width, leftImg->height), IPL_DEPTH_8U, 1);
		cvZero(disparityImg);

		// ----- run
		myTimer.opencvStart();
		stereoAlgo.census(leftImg, rightImg, disparityImg, 
							censusCorrelationWinSize, 
							censusMaxSearchDisparity, censusMinSearchDisparity, 
							censusLRcheckThr, censusUniquenessTest, 
							censusParameterWinSize);
		processingTime = myTimer.opencvStop();

		// ----- save the disparity image
		strFilename.Format(disparityImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		cvSaveImage(charArray, disparityImg);

		// ----- display
		displayResult();
		cvShowImage(WIN_DISPARITY_IMG, disparityImg);
		cvWaitKey(1);

		// ----- release
		cvReleaseImage(&leftImg);
		cvReleaseImage(&rightImg);
		cvReleaseImage(&disparityImg);
	}
}


void CMy110705_stereo_algoDlg::OnBnClickedButtonSad() {
	getInputParam();

	for(int i = imgFromIdx; i <= imgToIdx; i++) {

		// ----- load the left and right images
		strFilename.Format(leftImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		leftImg = cvLoadImage(charArray, 0);
		strFilename.Format(rightImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		rightImg = cvLoadImage(charArray, 0);
		
		// ----- init the disparity image
		disparityImg = cvCreateImage(cvSize(leftImg->width, leftImg->height), IPL_DEPTH_8U, 1);
		cvZero(disparityImg);

		// ----- run
		myTimer.opencvStart();
		stereoAlgo.sad(leftImg, rightImg, disparityImg, 
						sadCorrelationWinSize, 
						sadMaxSearchDisparity, sadMinSearchDisparity, 
						sadLRcheckThr, sadUniquenessTest);
		processingTime = myTimer.opencvStop();

		// ----- save the disparity image
		strFilename.Format(disparityImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		cvSaveImage(charArray, disparityImg);

		// ----- display
		displayResult();
		cvShowImage(WIN_DISPARITY_IMG, disparityImg);
		cvWaitKey(1);

		// ----- release
		cvReleaseImage(&leftImg);
		cvReleaseImage(&rightImg);
		cvReleaseImage(&disparityImg);
	}
}


void CMy110705_stereo_algoDlg::OnBnClickedButtonDynamicProgramming() {
	getInputParam();

	for(int i = imgFromIdx; i <= imgToIdx; i++) {

		// ----- load the left and right images
		strFilename.Format(leftImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		leftImg = cvLoadImage(charArray, 0);
		strFilename.Format(rightImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		rightImg = cvLoadImage(charArray, 0);
		
		// ----- init the disparity image
		disparityImg = cvCreateImage(cvSize(leftImg->width, leftImg->height), IPL_DEPTH_8U, 1);
		cvZero(disparityImg);

		// ----- run
		myTimer.opencvStart();
		stereoAlgo.dynamicProgramming(leftImg, rightImg, disparityImg,
								dynamicProgrammingOcclusionValue);
		processingTime = myTimer.opencvStop();

		// ----- save the disparity image
		strFilename.Format(disparityImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		cvSaveImage(charArray, disparityImg);

		// ----- display
		displayResult();
		cvShowImage(WIN_DISPARITY_IMG, disparityImg);
		cvWaitKey(1);

		// ----- release
		cvReleaseImage(&leftImg);
		cvReleaseImage(&rightImg);
		cvReleaseImage(&disparityImg);
	}
}


void CMy110705_stereo_algoDlg::OnBnClickedButtonSadOpencv() {
	getInputParam();

	for(int i = imgFromIdx; i <= imgToIdx; i++) {

		// ----- load the left and right images
		strFilename.Format(leftImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		leftImg = cvLoadImage(charArray, 0);
		strFilename.Format(rightImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		rightImg = cvLoadImage(charArray, 0);
		
		// ----- init the disparity image
		disparityImg = cvCreateImage(cvSize(leftImg->width, leftImg->height), IPL_DEPTH_8U, 1);
		cvZero(disparityImg);

		// ----- run
		myTimer.opencvStart();
		stereoAlgo.sadOpenCV(leftImg, rightImg, disparityImg,
						sadOpenCVpreFilterSize, sadOpenCVpreFilterCap,
						sadOpenCVSADWindowSize, sadOpenCVminDisparity, sadOpenCVnumberOfDisparities,
						sadOpenCVtextureThreshold, sadOpenCVuniquenessRatio, sadOpenCVspeckleWindowSize, sadOpenCVspeckleRange, sadOpenCVdisp12MaxDiff);
		processingTime = myTimer.opencvStop();

		// ----- save the disparity image
		strFilename.Format(disparityImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		cvSaveImage(charArray, disparityImg);

		// ----- display
		displayResult();
		cvShowImage(WIN_DISPARITY_IMG, disparityImg);
		cvWaitKey(1);

		// ----- release
		cvReleaseImage(&leftImg);
		cvReleaseImage(&rightImg);
		cvReleaseImage(&disparityImg);
	}
}


void CMy110705_stereo_algoDlg::OnBnClickedButtonGraphCutsOpencv() {
	getInputParam();

	for(int i = imgFromIdx; i <= imgToIdx; i++) {

		// ----- load the left and right images
		strFilename.Format(leftImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		leftImg = cvLoadImage(charArray, 0);
		strFilename.Format(rightImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		rightImg = cvLoadImage(charArray, 0);
		
		// ----- init the disparity image
		disparityImg = cvCreateImage(cvSize(leftImg->width, leftImg->height), IPL_DEPTH_8U, 1);		cvZero(disparityImg);
		tmpImg = cvCreateImage(cvSize(leftImg->width, leftImg->height), IPL_DEPTH_8U, 1);			cvZero(tmpImg);

		// ----- run
		myTimer.opencvStart();
		stereoAlgo.graphCutsOpenCV(leftImg, rightImg, disparityImg, tmpImg,
						graphCutsIthreshold, graphCutsInteractionRadius, graphCutsOcclusionCost, 
						graphCutsMinDisparity, graphCutsNumberOfDisparities, graphCutsMaxIters);
		processingTime = myTimer.opencvStop();

		// ----- save the disparity image
		strFilename.Format(disparityImgSeqPath, i);		WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		cvSaveImage(charArray, disparityImg);
		strFilename.Format(tmpImgSeqPath, i);			WideCharToMultiByte(CP_ACP, 0, strFilename, -1, charArray, 256, NULL, NULL); 		cvSaveImage(charArray, tmpImg);

		// ----- display
		displayResult();
		cvShowImage(WIN_DISPARITY_IMG, disparityImg);
		cvShowImage(WIN_TMP_IMG, tmpImg);
		cvWaitKey(1);

		// ----- release
		cvReleaseImage(&leftImg);
		cvReleaseImage(&rightImg);
		cvReleaseImage(&disparityImg);
		cvReleaseImage(&tmpImg);
	}
}


// =====================================================================================


void CMy110705_stereo_algoDlg::initVar() {
	isLoadImg = false;
	
	// ----- CENSUS -----
	censusMaxSearchDisparity			= CENSUS_MAX_SEARCH_DISPARITY;
	censusMinSearchDisparity			= CENSUS_MIN_SEARCH_DISPARITY;
	censusCorrelationWinSize			= CENSUS_CORRELATION_WIN_SIZE;		// window size for block matching
	censusParameterWinSize				= CENSUS_PARAMETER_WIN_SIZE;		// window size for census transform
	censusLRcheckThr					= CENSUS_LR_CHECK_THR;
	censusUniquenessTest				= CENSUS_UNIQUENESS_TEST;	

	// ----- SAD -----
	sadMaxSearchDisparity				= SAD_MAX_SEARCH_DISPARITY;
	sadMinSearchDisparity				= SAD_MIN_SEARCH_DISPARITY;
	sadCorrelationWinSize				= SAD_CORRELATION_WIN_SIZE;			// window size for block matching
	sadLRcheckThr						= SAD_LR_CHECK_THR;
	sadUniquenessTest					= SAD_UNIQUENESS_TEST;	

	// ----- DYNAMIC PROGRAMMING -----
	dynamicProgrammingOcclusionValue	= DYNAMIC_PROGRAMMING_OCCLUSION_VALUE;

	// ----- SAD - OpenCV -----
	sadOpenCVpreFilterSize				= SAD_OPENCV_PRE_FILTER_SIZE;
	sadOpenCVpreFilterCap				= SAD_OPENCV_PRE_FILTER_CAP;
	sadOpenCVSADWindowSize				= SAD_OPENCV_SAD_WINDOW_SIZE;
	sadOpenCVminDisparity				= SAD_OPENCV_MIN_DISPARITY;
	sadOpenCVnumberOfDisparities		= SAD_OPENCV_NUMBER_OF_DISPARITIES;
	sadOpenCVtextureThreshold			= SAD_OPENCV_TEXTURE_THRESHOLD;
	sadOpenCVuniquenessRatio			= SAD_OPENCV_UNIQUENESS_RATIO;
	sadOpenCVspeckleWindowSize			= SAD_OPENCV_SPECKLE_WINDOW_SIZE;
	sadOpenCVspeckleRange				= SAD_OPENCV_SPECKLE_RANGE;
	sadOpenCVdisp12MaxDiff				= SAD_OPENCV_DISP12MAXDIFF;

	// ----- GRAPH CUTS -----
	graphCutsIthreshold					= GRAPH_CUTS_OPENCV_ITHRESHOLD;			// threshold for piece-wise linear data cost function
	graphCutsInteractionRadius			= GRAPH_CUTS_OPENCV_INTERACTION_RADIUS;	// radius for smoothness cost function
	graphCutsOcclusionCost				= GRAPH_CUTS_OPENCV_OCCLUSION_COST;
	graphCutsMinDisparity				= GRAPH_CUTS_OPENCV_MIN_DISPARITY;
	graphCutsNumberOfDisparities		= GRAPH_CUTS_OPENCV_NUMBER_OF_DISPARITIES;
	graphCutsMaxIters					= GRAPH_CUTS_OPENCV_MAX_ITERS;
}


void CMy110705_stereo_algoDlg::initGUI() {
	CString s;

	// ----- CENSUS -----
	s.Format(L"%d", censusMaxSearchDisparity);			this->GetDlgItem(IDC_EDIT_CENSUS_MAX_SEARCH_DISPARITY)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", censusMinSearchDisparity);			this->GetDlgItem(IDC_EDIT_CENSUS_MIN_SEARCH_DISPARITY)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", censusCorrelationWinSize);			this->GetDlgItem(IDC_EDIT_CENSUS_CORRELATION_WIN_SIZE)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", censusParameterWinSize);			this->GetDlgItem(IDC_EDIT_CENSUS_PARAMETER_WIN_SIZE)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", censusLRcheckThr);					this->GetDlgItem(IDC_EDIT_CENSUS_LR_CHECK_THR)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", censusUniquenessTest);				this->GetDlgItem(IDC_EDIT_CENSUS_UNIQUENESS_TEST)->SetWindowTextW((LPCTSTR)s); 

	
	// ----- SAD -----
	s.Format(L"%d", sadMaxSearchDisparity);				this->GetDlgItem(IDC_EDIT_SAD_MAX_SEARCH_DISPARITY)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadMinSearchDisparity);				this->GetDlgItem(IDC_EDIT_SAD_MIN_SEARCH_DISPARITY)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadCorrelationWinSize);				this->GetDlgItem(IDC_EDIT_SAD_CORRELATION_WIN_SIZE)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadLRcheckThr);						this->GetDlgItem(IDC_EDIT_SAD_LR_CHECK_THR)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadUniquenessTest);					this->GetDlgItem(IDC_EDIT_SAD_UNIQUENESS_TEST)->SetWindowTextW((LPCTSTR)s); 
	

	// ----- DYNAMIC PROGRAMMING -----
	s.Format(L"%d", dynamicProgrammingOcclusionValue);	this->GetDlgItem(IDC_EDIT_DYNAMIC_PROGRAMMING_OCCLUSION_VALUE)->SetWindowTextW((LPCTSTR)s); 
	

	// ----- SAD - OpenCV -----
	s.Format(L"%d", sadOpenCVpreFilterSize);			this->GetDlgItem(IDC_EDIT_SAD_OPENCV_PRE_FILTER_SIZE)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadOpenCVpreFilterCap);				this->GetDlgItem(IDC_EDIT_SAD_OPENCV_PRE_FILTER_CAP)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadOpenCVSADWindowSize);			this->GetDlgItem(IDC_EDIT_SAD_OPENCV_SAD_WINDOW_SIZE)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadOpenCVminDisparity);				this->GetDlgItem(IDC_EDIT_SAD_OPENCV_MIN_DISPARITY)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadOpenCVnumberOfDisparities);		this->GetDlgItem(IDC_EDIT_SAD_OPENCV_NUMBER_OF_DISPARITIES)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadOpenCVtextureThreshold);			this->GetDlgItem(IDC_EDIT_SAD_OPENCV_TEXTURE_THRESHOLD)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadOpenCVuniquenessRatio);			this->GetDlgItem(IDC_EDIT_SAD_OPENCV_UNIQUENESS_RATIO)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadOpenCVspeckleWindowSize);		this->GetDlgItem(IDC_EDIT_SAD_OPENCV_SPECKLE_WINDOW_SIZE)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadOpenCVspeckleRange);				this->GetDlgItem(IDC_EDIT_SAD_OPENCV_SPECKLE_RANGE)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", sadOpenCVdisp12MaxDiff);			this->GetDlgItem(IDC_EDIT_SAD_OPENCV_DISP12MAXDIFF)->SetWindowTextW((LPCTSTR)s); 


	// ----- GRAPH CUTS -----
	s.Format(L"%d", graphCutsIthreshold);				this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_ITHRESHOLD)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", graphCutsInteractionRadius);		this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_INTERACTION_RADIUS)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", graphCutsOcclusionCost);			this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_OCCLUSION_COST)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", graphCutsMinDisparity);				this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_MIN_DISPARITY)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", graphCutsNumberOfDisparities);		this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_NUMBER_OF_DISPARITIES)->SetWindowTextW((LPCTSTR)s); 
	s.Format(L"%d", graphCutsMaxIters);					this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_MAX_ITERS)->SetWindowTextW((LPCTSTR)s); 
}


void CMy110705_stereo_algoDlg::getInputParam() {
	CString s;


	// ----- CENSUS -----
	this->GetDlgItem(IDC_EDIT_CENSUS_MAX_SEARCH_DISPARITY)->GetWindowTextW(s);				if (!s.IsEmpty())	censusMaxSearchDisparity = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_CENSUS_MIN_SEARCH_DISPARITY)->GetWindowTextW(s);				if (!s.IsEmpty())	censusMinSearchDisparity = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_CENSUS_CORRELATION_WIN_SIZE)->GetWindowTextW(s);				if (!s.IsEmpty())	censusCorrelationWinSize = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_CENSUS_PARAMETER_WIN_SIZE)->GetWindowTextW(s);				if (!s.IsEmpty())	censusParameterWinSize = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_CENSUS_LR_CHECK_THR)->GetWindowTextW(s);						if (!s.IsEmpty())	censusLRcheckThr = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_CENSUS_UNIQUENESS_TEST)->GetWindowTextW(s);					if (!s.IsEmpty())	censusUniquenessTest = (int)wcstod(s, NULL);


	// ----- SAD -----
	this->GetDlgItem(IDC_EDIT_SAD_MAX_SEARCH_DISPARITY)->GetWindowTextW(s);					if (!s.IsEmpty())	sadMaxSearchDisparity = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_MIN_SEARCH_DISPARITY)->GetWindowTextW(s);					if (!s.IsEmpty())	sadMinSearchDisparity = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_CORRELATION_WIN_SIZE)->GetWindowTextW(s);					if (!s.IsEmpty())	sadCorrelationWinSize = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_LR_CHECK_THR)->GetWindowTextW(s);							if (!s.IsEmpty())	sadLRcheckThr = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_UNIQUENESS_TEST)->GetWindowTextW(s);						if (!s.IsEmpty())	sadUniquenessTest = (int)wcstod(s, NULL);


	// ----- DYNAMIC PROGRAMMING -----
	this->GetDlgItem(IDC_EDIT_DYNAMIC_PROGRAMMING_OCCLUSION_VALUE)->GetWindowTextW(s);		if (!s.IsEmpty())	dynamicProgrammingOcclusionValue = (int)wcstod(s, NULL);
	

	// ----- SAD - OpenCV -----
	this->GetDlgItem(IDC_EDIT_SAD_OPENCV_PRE_FILTER_SIZE)->GetWindowTextW(s);				if (!s.IsEmpty())	sadOpenCVpreFilterSize = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_OPENCV_PRE_FILTER_CAP)->GetWindowTextW(s);				if (!s.IsEmpty())	sadOpenCVpreFilterCap = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_OPENCV_SAD_WINDOW_SIZE)->GetWindowTextW(s);				if (!s.IsEmpty())	sadOpenCVSADWindowSize = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_OPENCV_MIN_DISPARITY)->GetWindowTextW(s);					if (!s.IsEmpty())	sadOpenCVminDisparity = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_OPENCV_NUMBER_OF_DISPARITIES)->GetWindowTextW(s);			if (!s.IsEmpty())	sadOpenCVnumberOfDisparities = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_OPENCV_TEXTURE_THRESHOLD)->GetWindowTextW(s);				if (!s.IsEmpty())	sadOpenCVtextureThreshold = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_OPENCV_UNIQUENESS_RATIO)->GetWindowTextW(s);				if (!s.IsEmpty())	sadOpenCVuniquenessRatio = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_OPENCV_SPECKLE_WINDOW_SIZE)->GetWindowTextW(s);			if (!s.IsEmpty())	sadOpenCVspeckleWindowSize = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_OPENCV_SPECKLE_RANGE)->GetWindowTextW(s);					if (!s.IsEmpty())	sadOpenCVspeckleRange = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_SAD_OPENCV_DISP12MAXDIFF)->GetWindowTextW(s);					if (!s.IsEmpty())	sadOpenCVdisp12MaxDiff = (int)wcstod(s, NULL);


	// ----- GRAPH CUTS -----
	this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_ITHRESHOLD)->GetWindowTextW(s);				if (!s.IsEmpty())	graphCutsIthreshold = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_INTERACTION_RADIUS)->GetWindowTextW(s);		if (!s.IsEmpty())	graphCutsInteractionRadius = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_OCCLUSION_COST)->GetWindowTextW(s);			if (!s.IsEmpty())	graphCutsOcclusionCost = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_MIN_DISPARITY)->GetWindowTextW(s);			if (!s.IsEmpty())	graphCutsMinDisparity = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_NUMBER_OF_DISPARITIES)->GetWindowTextW(s);	if (!s.IsEmpty())	graphCutsNumberOfDisparities = (int)wcstod(s, NULL);
	this->GetDlgItem(IDC_EDIT_GRAPH_CUTS_OPENCV_MAX_ITERS)->GetWindowTextW(s);				if (!s.IsEmpty())	graphCutsMaxIters = (int)wcstod(s, NULL);
}


void CMy110705_stereo_algoDlg::displayResult() {
	CString s;
	s.Format(L"%d", processingTime);		GetDlgItem(IDC_STATIC_PROCESSING_TIME)->SetWindowTextW((LPCTSTR)s);
}
