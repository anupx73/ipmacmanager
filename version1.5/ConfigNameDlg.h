#pragma once

#include "BtnST.h"
#include "afxwin.h"
// CConfigNameDlg dialog

class CConfigNameDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfigNameDlg)

public:
	CConfigNameDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CConfigNameDlg();

// Dialog Data
	enum { IDD = IDD_CONFIG_NAME };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg LRESULT OnNcHitTest(UINT uParam, LONG lParam);
  BOOL PreTranslateMessage(MSG *pMsg);
  afx_msg void OnGoButtonClick();

	DECLARE_MESSAGE_MAP()

private:
	CBitmap m_BmpImg[3];
	CSize m_SizBmpImg[3];
  CButtonST *m_pBtnGo;

  void LoadImagesFromResource();
  void CreateButton();

  void AdjustWindowPosition(BITMAP *bm);
  void CloseConfigNameDialog();

public:
  CEdit mEdit;
  void Initialize();
};
