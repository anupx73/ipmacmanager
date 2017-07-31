// CUtilityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPMACManager.h"
#include "UtilityDlg.h"
#include "IPMACManagerDlg.h"

#define BTN_ID_OK     701
#define BTN_ID_CANCEL 702
#define BTN_ID_RADIO_MAC 703 //+4 reserved
#define BTN_ID_VIEW_DETAILS 707

extern CIPMACManagerDlg *g_pIPMACManagerDlg;
extern TCHAR g_chRandomSecondNibble[];

// CUtilityDlg dialog
//rect text
CRect rcTextHeading(12, 26, 372, 41);
CRect rcText(12, 52, 368, 99);
CRect rcTextChoose(12, 102, 372, 117);
CRect rcBackupHeading(12, 50, 372, 70);
CRect rcDetailsText (12, 122, 60, 134);
CRect rcDetailsData (12, 138, 98, 182);
CRect rcDetailsValue(100, 138, 270, 182);

//rect buttons
CRect rcRadioBtn(32, 122, 182, 138);
CRect rcBtnOK(287, 140, 372, 165);
CRect rcBtnShowMore(12, 181, 68, 195);

#define BUTTON_RADIO_Y_OFFSET 17
#define BTN_Y_OFFSET 27

//text
CString g_cstrTextHeading1 = _T("WARNING | MAC Address is not compatible");
CString g_cstrTextHeading2 = _T("Select desired backup to apply");
//CString g_cstrTextHeading3 = _T("Config name         Time                                 IP address");
CString g_cstrTextHeading3 = _T("Configuration name");

CString g_cstrTextNotify = _T("For wireless adapter under Windows 7 or Higher, the acceptable range of spoofed MAC address is limited. The second nibble of the MAC Address must be 2, 6, A or E.");
CString g_cstrTextChoose = _T("Choose correct address");

IMPLEMENT_DYNAMIC(CUtilityDlg, CDialog)

CUtilityDlg::CUtilityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUtilityDlg::IDD, pParent)
{
  m_pBtnOk = NULL;
  m_pBtnCancel = NULL;
  m_pBtnViewDetails = NULL;
  m_cstrMACAddress = _T("");
  m_pBtnMAC1 = NULL;
  m_pBtnMAC2 = NULL;
  m_pBtnMAC3 = NULL;
  m_pBtnMAC4 = NULL;
  m_iScreenParam = NO_SCREEN;
  m_cstrIPBackupData  = _T("");
  m_cstrIPBackupValue = _T("");
  m_iCurrentSel = 0;
}

CUtilityDlg::~CUtilityDlg()
{
}

void CUtilityDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_IP_BACKUPS, m_ListIPBackups);
}


BEGIN_MESSAGE_MAP(CUtilityDlg, CDialog)
  ON_WM_PAINT()
  ON_WM_CTLCOLOR()
  ON_WM_LBUTTONDOWN()
  ON_BN_CLICKED(BTN_ID_OK, OnOkButtonClick)
  ON_BN_CLICKED(BTN_ID_CANCEL, OnCancelButtonClick)
  ON_BN_CLICKED(BTN_ID_VIEW_DETAILS, OnIPViewDetailsButtonClick)
  ON_LBN_SELCHANGE(IDC_LIST_IP_BACKUPS, &CUtilityDlg::OnLbnSelchangeListIpBackups)
END_MESSAGE_MAP()


// CUtilityDlg message handlers
BOOL CUtilityDlg::OnInitDialog()
{
  CDialog::OnInitDialog();
  
  m_brush.CreateSolidBrush(RGB(255, 255, 255));
  LoadImagesFromResource();
  
  m_ListIPBackups.ShowWindow(FALSE);
  CreateButtons();

  return TRUE;
}

HBRUSH CUtilityDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
  if (nCtlColor == CTLCOLOR_DLG || nCtlColor == CTLCOLOR_STATIC)
  {
    // set up the colors here to match dialog background
    //pDC->SetTextColor(RGB(200,0,0));
    pDC->SetBkColor(RGB(255,255,255));
    return (HBRUSH)m_brush.GetSafeHandle();
  }

  return m_brush;
}

BOOL CUtilityDlg::PreTranslateMessage(MSG *pMsg)
{
  if((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
  {
    switch(pMsg->wParam)
    {
    case VK_ESCAPE:
    case VK_RETURN:
      return TRUE;

    default:
      return CDialog::PreTranslateMessage(pMsg);
    }
  }
  else
  {
    return CDialog::PreTranslateMessage(pMsg);
  }
}

void CUtilityDlg::OnPaint()
{
	CPaintDC dc(this);
  CDC pdc;
  pdc.CreateCompatibleDC(&dc);
  CBitmap *oldBmp = pdc.SelectObject(&m_BmpImg[0]);
  dc.BitBlt(0, 0, m_SizBmpImg[0].cx, m_SizBmpImg[0].cy, &pdc, 0, 0, SRCCOPY);
  dc.SelectObject(&oldBmp);
  
  //text
  dc.SetBkMode(TRANSPARENT);

  if(m_iScreenParam == WIRELESS_MAC)
  {
    dc.SelectObject(&g_pIPMACManagerDlg->m_txtHeadingFont);
    dc.SetTextColor(RGB(255, 255, 255));
    dc.DrawText(g_cstrTextHeading1, g_cstrTextHeading1.GetLength(), rcTextHeading, DT_LEFT);

    dc.SetTextColor(RGB(0, 0, 0));
    dc.SelectObject(&g_pIPMACManagerDlg->m_txtFont);
    //dc.FillSolidRect(rcText, RGB(200,0,0));
    dc.DrawText(g_cstrTextNotify, g_cstrTextNotify.GetLength(), rcText, DT_LEFT | DT_WORDBREAK);

    dc.SelectObject(&g_pIPMACManagerDlg->m_txtHeadingFont);
    dc.SetTextColor(RGB(223, 64, 2));
    dc.DrawText(g_cstrTextChoose, g_cstrTextChoose.GetLength(), rcTextChoose, DT_LEFT);
  }
  else if(m_iScreenParam == IP_RESTORE)
  {
    dc.SelectObject(&g_pIPMACManagerDlg->m_txtHeadingFont);
    dc.SetTextColor(RGB(255, 255, 255));
    dc.DrawText(g_cstrTextHeading2, g_cstrTextHeading2.GetLength(), rcTextHeading, DT_LEFT);

    dc.SetTextColor(RGB(223, 64, 2));
    dc.DrawText(g_cstrTextHeading3, g_cstrTextHeading3.GetLength(), rcBackupHeading, DT_LEFT);

    //dc.FillSolidRect(rcDetails, RGB(223, 64, 2));
    CString cstr = _T("Details");
    dc.SelectObject(&g_pIPMACManagerDlg->m_txtFont);
    dc.DrawText(cstr, cstr.GetLength(), rcDetailsText, DT_LEFT);
    
    //dc.FillSolidRect(rcDetailsData, RGB(223, 64, 2));
    //dc.FillSolidRect(rcDetailsValue, RGB(23, 223, 2));

    dc.SetTextColor(RGB(0, 0, 0));
    dc.DrawText(m_cstrIPBackupData, m_cstrIPBackupData.GetLength(), rcDetailsData, DT_LEFT);
    dc.DrawText(m_cstrIPBackupValue, m_cstrIPBackupValue.GetLength(), rcDetailsValue, DT_LEFT);
  }
}


void CUtilityDlg::LoadImagesFromResource()
{
  BITMAP bm = {0};
  //Back img
  m_BmpImg[0].LoadBitmap(IDB_BITMAP4);
  m_BmpImg[0].GetBitmap(&bm);
  m_SizBmpImg[0] = CSize(bm.bmWidth, bm.bmHeight);
  //SetWindowPos(&wndBottom, 55, 50, bm.bmWidth, bm.bmHeight, SWP_SHOWWINDOW);

  AdjustWindowPosition(&bm);
  m_ListIPBackups.SetWindowPos(&wndTop, 0, 0, bm.bmWidth - 20, bm.bmHeight - 150, SWP_NOMOVE);
}

void CUtilityDlg::OnOkButtonClick()
{
  bool bReturn = false;
  if(g_pIPMACManagerDlg->m_eActiveTab == TAB_MAC)
  {
    bReturn = ChangeMACAddress();
  }
  else
  {
    bReturn = ChangeIPAddress();
  }

  if(bReturn == false)
    return;

  g_pIPMACManagerDlg->EnableWindow();
  ShowWindow(FALSE);
}

void CUtilityDlg::OnCancelButtonClick()
{
  g_pIPMACManagerDlg->EnableWindow();
  ShowWindow(FALSE);
}

void CUtilityDlg::SetMACAddress(CString cstrMAC)
{
  m_cstrMACAddress = cstrMAC;
}

void CUtilityDlg::CreateMACAddressRadioButtons()
{
  m_ListIPBackups.ShowWindow(FALSE);
  m_pBtnViewDetails->ShowWindow(FALSE);

  if(m_pBtnMAC1 == NULL)
  {
    m_pBtnMAC1 = new CButton;
    if(m_pBtnMAC1 != NULL)
    {
      m_pBtnMAC1->Create(_T(""), WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, rcRadioBtn, this, BTN_ID_RADIO_MAC);
      m_pBtnMAC1->SetFont(&g_pIPMACManagerDlg->m_txtFont);
    }
  }

  if(m_pBtnMAC2 == NULL)
  {
    rcRadioBtn.top    += BUTTON_RADIO_Y_OFFSET;
    rcRadioBtn.bottom += BUTTON_RADIO_Y_OFFSET;
    m_pBtnMAC2 = new CButton;
    if(m_pBtnMAC2 != NULL)
    {
      m_pBtnMAC2->Create(_T(""), WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, rcRadioBtn, this, BTN_ID_RADIO_MAC+1);
      m_pBtnMAC2->SetFont(&g_pIPMACManagerDlg->m_txtFont);
    }
  }

  if(m_pBtnMAC3 == NULL)
  {
    rcRadioBtn.top    += BUTTON_RADIO_Y_OFFSET;
    rcRadioBtn.bottom += BUTTON_RADIO_Y_OFFSET;
    m_pBtnMAC3 = new CButton;
    if(m_pBtnMAC3 != NULL)
    {
      m_pBtnMAC3->Create(_T(""), WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, rcRadioBtn, this, BTN_ID_RADIO_MAC+2);
      m_pBtnMAC3->SetFont(&g_pIPMACManagerDlg->m_txtFont);
    }
  }

  if(m_pBtnMAC4 == NULL)
  {
    rcRadioBtn.top    += BUTTON_RADIO_Y_OFFSET;
    rcRadioBtn.bottom += BUTTON_RADIO_Y_OFFSET;
    m_pBtnMAC4 = new CButton;
    if(m_pBtnMAC4 != NULL)
    {
      m_pBtnMAC4->Create(_T(""), WS_CHILD|WS_VISIBLE|BS_AUTORADIOBUTTON, rcRadioBtn, this, BTN_ID_RADIO_MAC+3);
      m_pBtnMAC4->SetFont(&g_pIPMACManagerDlg->m_txtFont);
    }
  }
}

void CUtilityDlg::PopulateMACAddressOnRadioButtons()
{    
  CString cstrNewMAC = m_cstrMACAddress;
  
  cstrNewMAC.SetAt(1, g_chRandomSecondNibble[0]);
  m_pBtnMAC1->SetWindowText(cstrNewMAC);
  m_pBtnMAC1->ShowWindow(TRUE);
  m_pBtnMAC1->SetCheck(BST_CHECKED);

  cstrNewMAC.SetAt(1, g_chRandomSecondNibble[1]);
  m_pBtnMAC2->SetWindowText(cstrNewMAC);
  m_pBtnMAC2->ShowWindow(TRUE);

  cstrNewMAC.SetAt(1, g_chRandomSecondNibble[2]);
  m_pBtnMAC3->SetWindowText(cstrNewMAC);
  m_pBtnMAC3->ShowWindow(TRUE);

  cstrNewMAC.SetAt(1, g_chRandomSecondNibble[3]);
  m_pBtnMAC4->SetWindowText(cstrNewMAC);
  m_pBtnMAC4->ShowWindow(TRUE);
}

LRESULT CUtilityDlg::OnNcHitTest(UINT uParam, LONG lParam)
{
  int xPos = LOWORD(lParam);
  int yPos = HIWORD(lParam);
  UINT nHitTest = CDialog::OnNcHitTest(CSize(xPos, yPos));

  return (nHitTest == HTCLIENT) ? HTCAPTION : nHitTest;
}

void CUtilityDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
  //CString cstr;
  //cstr.Format(_T("X: %d, Y: %d"), point.x, point.y);
  //AfxMessageBox(cstr);

  if((point.x >= 0 && point.x <= 380) && (point.y >= 0 && point.y <= 12))
  {
    PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
  }

  CDialog::OnLButtonDown(nFlags, point);
}


void CUtilityDlg::CreateButtons()
{
  m_pBtnOk = new CButtonST;
  if(m_pBtnOk != NULL)
  {
    m_pBtnOk->Create(_T(""), WS_CHILD | WS_GROUP | WS_TABSTOP, rcBtnOK, this, BTN_ID_OK);
    m_pBtnOk->SetFont(&g_pIPMACManagerDlg->m_txtFont);
    m_pBtnOk->DrawBorder(FALSE);

    m_pBtnOk->SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(197, 59, 5));
    m_pBtnOk->SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(223, 64, 2));
    m_pBtnOk->SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(223, 64, 2));

    m_pBtnOk->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
    m_pBtnOk->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(255, 255, 255));
    m_pBtnOk->SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255));
  }

  m_pBtnCancel = new CButtonST;
  if(m_pBtnCancel != NULL)
  {
    rcBtnOK.top    += BTN_Y_OFFSET;
    rcBtnOK.bottom += BTN_Y_OFFSET;

    m_pBtnCancel->Create(_T(""), WS_CHILD | WS_GROUP | WS_TABSTOP, rcBtnOK, this, BTN_ID_CANCEL);
    m_pBtnCancel->SetFont(&g_pIPMACManagerDlg->m_txtFont);
    m_pBtnCancel->DrawBorder(FALSE);

    m_pBtnCancel->SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(197, 59, 5));
    m_pBtnCancel->SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(223, 64, 2));
    m_pBtnCancel->SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(223, 64, 2));

    m_pBtnCancel->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
    m_pBtnCancel->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(255, 255, 255));
    m_pBtnCancel->SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255));
  }

  m_pBtnViewDetails = new CButtonST;
  if(m_pBtnViewDetails != NULL)
  {
    m_pBtnViewDetails->Create(_T("Show more"), WS_CHILD | WS_GROUP | WS_TABSTOP, rcBtnShowMore, this, BTN_ID_VIEW_DETAILS);
    m_pBtnViewDetails->SetFont(&g_pIPMACManagerDlg->m_txtFont);
    
    m_pBtnViewDetails->DrawTransparent();
    m_pBtnViewDetails->DrawBorder(FALSE);
    m_pBtnViewDetails->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(223, 64, 2));
    m_pBtnViewDetails->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(223, 64, 2));
    m_pBtnViewDetails->SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(223, 64, 2));
  }
}

void CUtilityDlg::SetButtonLabels()
{
  if((m_pBtnOk != NULL) && (::IsWindow(m_pBtnOk->GetSafeHwnd()) == TRUE))
  {
    if(m_iScreenParam == WIRELESS_MAC)
      m_pBtnOk->SetWindowText(_T("OK"));
    else if(m_iScreenParam == IP_RESTORE)
      m_pBtnOk->SetWindowText(_T("Apply"));

    m_pBtnOk->ShowWindow(TRUE);
  }

  if((m_pBtnCancel != NULL) && (::IsWindow(m_pBtnCancel->GetSafeHwnd()) == TRUE))
  {
    if(m_iScreenParam == WIRELESS_MAC)
      m_pBtnCancel->SetWindowText(_T("No Thanks"));
    else if(m_iScreenParam == IP_RESTORE)
      m_pBtnCancel->SetWindowText(_T("Back"));

    m_pBtnCancel->ShowWindow(TRUE);
  }
}

void CUtilityDlg::SetScreenParameters(short iParam, CString cstrAdditional)
{
  m_iScreenParam = iParam;

  g_pIPMACManagerDlg->EnableWindow(FALSE);

  SetButtonLabels();

  if(iParam == WIRELESS_MAC)
  {
    CreateMACAddressRadioButtons();
    PopulateMACAddressOnRadioButtons();
  }
  else if(iParam == IP_RESTORE)
  {
    HideWirelessMACControls();
    PopulateBackupIPData(cstrAdditional);
  }
}

void CUtilityDlg::PopulateBackupIPData(CString cstrGUID)
{
  m_ListIPBackups.ShowWindow(TRUE);
  //m_pBtnViewDetails->ShowWindow(TRUE);
  m_ListIPBackups.ResetContent();
  m_arrBackupFileName.RemoveAll();

  TCHAR szCurrentPath[MAX_PATH] = _T("");
  ::GetCurrentDirectory(MAX_PATH, szCurrentPath);
  CString cstrBackupFolder(szCurrentPath);
  cstrBackupFolder += _T("\\Backups\\");
  CString cstrFile = cstrBackupFolder + _T("*.ipm");

  WIN32_FIND_DATA FindFileData = {0};
  HANDLE hFind = FindFirstFile(cstrFile, &FindFileData);
  if(hFind != INVALID_HANDLE_VALUE)
  {
    CString cstrFullFile = cstrBackupFolder + CString(FindFileData.cFileName);
    ParseIPBackupFiles(cstrFullFile, cstrGUID);

    while(FindNextFile(hFind, &FindFileData) != 0)
    {
      cstrFullFile = cstrBackupFolder + CString(FindFileData.cFileName);
      ParseIPBackupFiles(cstrFullFile, cstrGUID);
    }
    FindClose(hFind);
    m_ListIPBackups.SetCurSel(0);
    GetIPBackupDetails(0);
  }
}

void CUtilityDlg::ParseIPBackupFiles(CString cstrFileName, CString cstrGUID)
{
  CStdioFile oFile;
  BOOL bReturn = oFile.Open((LPCTSTR)cstrFileName, CFile::modeRead);
  if(bReturn == TRUE)
  {
    CString cstrData;
    oFile.ReadString(cstrData);

    //{GUID}|Name|Time|DHCP|IP|Subnet|Gateway|DNS
    if(cstrData.IsEmpty() == false)
    {
      CString cstrFileGUID;
      AfxExtractSubString(cstrFileGUID, cstrData, 0, '|');
      if(cstrFileGUID == cstrGUID)
      {
        CString cstrBackupName;
        AfxExtractSubString(cstrBackupName, cstrData, 1, '|');
        m_ListIPBackups.AddString(cstrBackupName);
        m_arrBackupFileName.Add(cstrFileName);
      }
    }

    oFile.Close();
  }
}

void CUtilityDlg::OnIPViewDetailsButtonClick()
{

}

void CUtilityDlg::AdjustWindowPosition(BITMAP *bm)
{
  //Window pos
  HWND hwndOwner = NULL; 
  RECT rc, rcDlg, rcOwner; 

  if ((hwndOwner = ::GetParent(this->m_hWnd)) == NULL) 
  {
    hwndOwner = ::GetDesktopWindow(); 
  }

  ::GetWindowRect(hwndOwner, &rcOwner); 
  ::GetWindowRect(this->m_hWnd, &rcDlg); 
  ::CopyRect(&rc, &rcOwner); 

  // Offset the owner and dialog box rectangles so that right and bottom 
  // values represent the width and height, and then offset the owner again 
  // to discard space taken up by the dialog box. 

  OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
  OffsetRect(&rc, -rc.left, -rc.top); 
  OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 

  SetWindowPos(&wndTop, rcOwner.left + (rc.right / 2), rcOwner.top + (rc.bottom / 2), bm->bmWidth, bm->bmHeight, SWP_SHOWWINDOW); 
}

bool CUtilityDlg::ChangeMACAddress()
{
  CString cstrMAC;
  if(m_pBtnMAC1->GetCheck() == BST_CHECKED)
    m_pBtnMAC1->GetWindowText(cstrMAC);
  else if(m_pBtnMAC2->GetCheck() == BST_CHECKED)
    m_pBtnMAC2->GetWindowText(cstrMAC);
  else if(m_pBtnMAC3->GetCheck() == BST_CHECKED)
    m_pBtnMAC3->GetWindowText(cstrMAC);
  else if(m_pBtnMAC4->GetCheck() == BST_CHECKED)
    m_pBtnMAC4->GetWindowText(cstrMAC);
  else
  {
    AfxMessageBox(_T("Choose your preferred MAC Address and retry"));
    return false;
  }

  g_pIPMACManagerDlg->ChangeMACAddress(cstrMAC);
  return true;
}

bool CUtilityDlg::ChangeIPAddress()
{
  if(m_iCurrentSel == -1)
  {
    AfxMessageBox(_T("Choose preferred IP config and retry"));
    return false;
  }
  if(m_arrBackupFileName.GetCount() <= 0)
  {
    AfxMessageBox(_T("No backups found to be applied"));
    return false;
  }

  CString cstrFileName = m_arrBackupFileName.GetAt(m_iCurrentSel);
  CStdioFile oFile;
  BOOL bReturn = oFile.Open((LPCTSTR)cstrFileName, CFile::modeRead);
  if(bReturn == TRUE)
  {
    CString cstrData;
    oFile.ReadString(cstrData);
    //{GUID}|Name|Time|DHCP|IP|Subnet|Gateway|DNS
    if(cstrData.IsEmpty() == false)
    {
      g_pIPMACManagerDlg->ChangeIPAddress(cstrFileName, cstrData);
    }
    oFile.Close();
  }

  return true;
}

void CUtilityDlg::GetIPBackupDetails(short iIndex)
{
  m_cstrIPBackupData.Empty();
  m_cstrIPBackupValue.Empty();

  CString cstrFileName = m_arrBackupFileName.GetAt(iIndex);
  CStdioFile oFile;
  BOOL bReturn = oFile.Open((LPCTSTR)cstrFileName, CFile::modeRead);
  if(bReturn == TRUE)
  {
    CString cstrData;
    oFile.ReadString(cstrData);
    //{GUID}|Name|Time|DHCP|IP|Subnet|Gateway|DNS
    if(cstrData.IsEmpty() == false)
    {
      CString cstrTime, cstrDHCP, cstrIP;
      AfxExtractSubString(cstrTime, cstrData, 2, '|');
      AfxExtractSubString(cstrDHCP, cstrData, 3, '|');
      AfxExtractSubString(cstrIP, cstrData, 4, '|');

      (cstrDHCP == _T("1")) ? cstrDHCP = _T("Yes") : cstrDHCP = _T("No");

      m_cstrIPBackupData = CString(_T("Backup time")) + _T("\r\n") + 
                           CString(_T("DHCP Enable")) + _T("\r\n") +
                           CString(_T("IP Address"))  + _T("\r\n");

      m_cstrIPBackupValue = cstrTime + _T("\r\n") + 
                            cstrDHCP + _T("\r\n") + 
                            cstrIP;

      InvalidateRect(rcDetailsData);
      InvalidateRect(rcDetailsValue);
      UpdateWindow();
    }
    oFile.Close();
  }
}


void CUtilityDlg::OnLbnSelchangeListIpBackups()
{
  int iIndex = m_ListIPBackups.GetCurSel();
  if(iIndex == -1)
  {
    return;
  }

  if(m_iCurrentSel != iIndex)
  {
    m_iCurrentSel = iIndex;
    GetIPBackupDetails(iIndex);
  }  
}

void CUtilityDlg::HideWirelessMACControls()
{
  if((m_pBtnMAC1 != NULL) && (::IsWindow(m_pBtnMAC1->GetSafeHwnd()) == TRUE))
  {
    //if(m_pBtnMAC1->IsWindowVisible() == TRUE)
      m_pBtnMAC1->ShowWindow(FALSE);
  }
  if((m_pBtnMAC2 != NULL) && (::IsWindow(m_pBtnMAC2->GetSafeHwnd()) == TRUE))
  {
    //if(m_pBtnMAC2->IsWindowVisible() == TRUE)
      m_pBtnMAC2->ShowWindow(FALSE);
  }
  if((m_pBtnMAC3 != NULL) && (::IsWindow(m_pBtnMAC3->GetSafeHwnd()) == TRUE))
  {
    //if(m_pBtnMAC3->IsWindowVisible() == TRUE)
      m_pBtnMAC3->ShowWindow(FALSE);
  }
  if((m_pBtnMAC4 != NULL) && (::IsWindow(m_pBtnMAC4->GetSafeHwnd()) == TRUE))
  {
    //if(m_pBtnMAC4->IsWindowVisible() == TRUE)
      m_pBtnMAC4->ShowWindow(FALSE);
  }
}
