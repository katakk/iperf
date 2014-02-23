#pragma once


// CIperfView
class CIperfViewItem;

class CIperfView : public CWnd
{
	DECLARE_DYNAMIC(CIperfView)

public:
	CIperfView();
	virtual ~CIperfView();

protected:
	CMapStringToOb m_transaction;
	DECLARE_MESSAGE_MAP()
public:
	void PaintItems(CPaintDC *pDC, CIperfViewItem *pa);
	afx_msg void OnPaint();
	int AddItem(LPCTSTR peer, double time,double speed);
};


