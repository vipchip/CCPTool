#pragma once


// CTextProgressCtrl

class CTextProgressCtrl : public CProgressCtrl
{
	DECLARE_DYNAMIC(CTextProgressCtrl)

public:
	CTextProgressCtrl();
	virtual ~CTextProgressCtrl();

	int GetPercent();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
};


