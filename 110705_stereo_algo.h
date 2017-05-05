
// 110705_stereo_algo.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMy110705_stereo_algoApp:
// See 110705_stereo_algo.cpp for the implementation of this class
//

class CMy110705_stereo_algoApp : public CWinApp
{
public:
	CMy110705_stereo_algoApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMy110705_stereo_algoApp theApp;