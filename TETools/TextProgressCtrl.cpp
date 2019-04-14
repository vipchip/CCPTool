// TextProgressCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TETools.h"
#include "TextProgressCtrl.h"


// CTextProgressCtrl

IMPLEMENT_DYNAMIC(CTextProgressCtrl, CProgressCtrl)

CTextProgressCtrl::CTextProgressCtrl()
{

}

CTextProgressCtrl::~CTextProgressCtrl()
{
}


BEGIN_MESSAGE_MAP(CTextProgressCtrl, CProgressCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CTextProgressCtrl message handlers




BOOL CTextProgressCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return CProgressCtrl::OnEraseBkgnd(pDC);
}


void CTextProgressCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CProgressCtrl::OnPaint() for painting messages
	CRect rect;
	GetClientRect(&rect);

	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap *pOldBitmap = MemDC.SelectObject(&bitmap);
	MemDC.FillSolidRect(&rect, RGB(255, 255, 255));
	int nPercent = GetPercent();
	int nSeperator = (int)((rect.Width() * nPercent) / 100);
	CRect rcLeft, rcRight;
	rcLeft.left = 0;
	rcLeft.right = rcLeft.left + nSeperator;
	rcLeft.top = rect.top;
	rcLeft.bottom = rect.bottom;
	MemDC.FillSolidRect(&rcLeft, RGB(50, 125, 125));


	CString strPercent;
	strPercent.Format(_T("%d%%"), nPercent);
	MemDC.SetBkMode(TRANSPARENT);                            //不改变文本背景
	MemDC.DrawText(strPercent, rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	dc.SelectObject(pOldBitmap);
	bitmap.DeleteObject();
}


int CTextProgressCtrl::GetPercent()
{
	int nLower, nUpper, nPos;
	GetRange(nLower, nUpper);
	nPos = GetPos();
	return (nPos * 100) / (nUpper - nLower);
}
