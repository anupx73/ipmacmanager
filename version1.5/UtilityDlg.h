#pragma once


// CUtilityDlg dialog
#include "BtnST.h"
#include "afxwin.h"

class CUtilityDlg : public CDialog
{
	DECLARE_DYNAMIC(CUtilityDlg)

public:
	CUtilityDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CUtilityDlg();

// Dialog Data
	enum { IDD = IDD_UTILITY_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG *pMsg);
	afx_msg void OnPaint();
  afx_msg void OnOkButtonClick();
  afx_msg void OnCancelButtonClick();
  afx_msg void OnIPViewDetailsButtonClick();
  HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
  afx_msg LRESULT OnNcHitTest(UINT uParam, LONG lParam);
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  
	DECLARE_MESSAGE_MAP()

private:
	CBitmap m_BmpImg[1];
	CSize m_SizBmpImg[1];

  void LoadImagesFromResource();

  CButtonST *m_pBtnOk;
  CButtonST *m_pBtnCancel;
  CButtonST *m_pBtnViewDetails;
  
  CButton *m_pBtnMAC1;
  CButton *m_pBtnMAC2;
  CButton *m_pBtnMAC3;
  CButton *m_pBtnMAC4;
  
  CString m_cstrMACAddress;
  void CreateMACAddressRadioButtons();
  CBrush m_brush;

  void CreateButtons();
  void SetButtonLabels();

  short m_iScreenParam;
  CListBox m_ListIPBackups;

  void PopulateBackupIPData(CString cstrGUID);
  void ParseIPBackupFiles(CString cstrFileName, CString cstrGUID);

  CStringArray m_arrBackupFileName;
  void AdjustWindowPosition(BITMAP *bm);
  bool ChangeMACAddress();
  bool ChangeIPAddress();
  CString m_cstrIPBackupData;
  CString m_cstrIPBackupValue;
  void GetIPBackupDetails(short iIndex);
  void HideWirelessMACControls();
  int m_iCurrentSel;

public:
  void SetMACAddress(CString cstrMAC);
  void PopulateMACAddressOnRadioButtons();
  void SetScreenParameters(short iParam, CString cstrAdditional = _T(""));
  
  afx_msg void OnLbnSelchangeListIpBackups();
};

