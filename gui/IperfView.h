#pragma once


// CIperfView

class CIperfView : public CWnd
{
	DECLARE_DYNAMIC(CIperfView)

public:
	CIperfView();
	virtual ~CIperfView();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};


