// IPMACManagerDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "BtnST.h"
#include "MACAddrEdit.h"
#include "UtilityDlg.h"
#include "ConfigNameDlg.h"
#include "LogWrite.h"

enum eActiveTab{
  TAB_IP,
  TAB_MAC
};

enum eUtilityScreen{
  NO_SCREEN,
  WIRELESS_MAC,
  IP_RESTORE
};

// CIPMACManagerDlg dialog
class CIPMACManagerDlg : public CDialog
{
// Construction
public:
	CIPMACManagerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IPMACMANAGER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  afx_msg LRESULT OnNcHitTest(UINT uParam, LONG lParam);
  BOOL PreTranslateMessage(MSG *pMsg);
	DECLARE_MESSAGE_MAP()
  afx_msg void OnLbnSelchangeListConn();
  
  afx_msg void OnSettingsChangeButtonClick();
  afx_msg void OnSettingsRestoreButtonClick();

  afx_msg void OnMACGenerateButtonClick();
  afx_msg void OnMinimizeButtonClick();
  afx_msg void OnCloseButtonClick();

private:
	CBitmap m_BmpImg[3];
	CSize m_SizBmpImg[3];
  
  CListBox mListConnections;

  void LoadImagesFromResource();
  void RetrieveAdaptersInfo(bool bRefresh = false, CString cstrAdapter = _T(""));
  void RetrieveConnectionInfo(short iAdapterIndex, CString cstrAdapterGUID, bool bRefresh);

  CStringArray m_arrAdapterData[10];
  CString m_cstrDisplayData;
  CString m_cstrDisplayValue;
  
  CMACAddrEdit mNewMAC;
  
  CButtonST *m_pBtnSettingsChange;
  CButtonST *m_pBtnSettingsRestore;
  CButtonST *m_pBtnMACGenerate;
  
  CButtonST *m_pBtnClose;
  CButtonST *m_pBtnMinimize;

  void InitializeControls();
  void CreateRequiredFonts();
  void SetControlsState();

  void RestartAdapter(CString cstrAdapter);
  bool CheckMACSpoofed(CString cstrGUID = _T(""));
  void RestoreMACAddress();
  CString GenerateRandomMACAddress();
  void RefreshAdapterInfo(bool bCalledFromOnInit = false);

  int m_iCurrentConnectionIndex;

  void SetFirstSelectionForMAC();
  void GetOSVersion();
  bool m_bIsWindows7OrHigher;
  bool m_bWarnWirelessMACChange;
  
  CUtilityDlg *m_pUtilityDlg;
  CConfigNameDlg *m_pConfigNameDlg;

  void ConnectionSelectionChange();
  
  void OnIPSettingsChange();
  void OnMACSettingsChange();
  void OnIPSettingsRestore();
  void OnMACSettingsRestore();

  void OpenIPTab();
  void OpenMACTab();

  CString BuildIPBackupFileName();
  bool IsValidMAC(CString cstrMAC);
  void CreateUtilityDialog();
  CString m_cstrCurrWriteTime;
  void WriteLastToRegistry(CString cstrData);
  void ReadLastBackupData();
  CString m_cstrLastBackup;
  void RefreshLastBackupArea(bool bOnlyInvalidate = false);
  void AskForConfigName();
  BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege);

public:
  CFont m_txtHeadingFont;
  CFont m_txtFont;
  bool m_bCancelNewCompatibleMAC;
  void ChangeMACAddress(CString csNewMACAdd);
  void CreateIPBackup(CString cstrConfigName);
  eActiveTab m_eActiveTab;
  void ChangeIPAddress(CString cstrFileName, CString cstrBackupData);
  CLogWrite m_LogWrite;
};
