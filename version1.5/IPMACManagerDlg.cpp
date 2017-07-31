// IPMACManagerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPMACManager.h"
#include "IPMACManagerDlg.h"
#include <netcon.h>
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define BTN_ID_MAC_CHANGE 601
#define BTN_ID_MAC_RESTORE 602
#define BTN_ID_MAC_GENERATE 603
#define BTN_ID_MINIMIZE 604
#define BTN_ID_CLOSE 605

//text rect
CRect rcTabIP(20, 127, 81, 144);
CRect rcTabMAC(106, 127, 180, 144);
CRect rcRefresh(11, 141, 483, 300);
CRect rcData(17, 160, 480, 260);
CRect rcSpoofStatus(141, 200, 223, 228);
CRect rcNewMACBlock(17, 242, 280, 257);
CRect rcLastConfig(17, 265, 380, 280);
CRect rcDataBlock(17, 160, 120, 240);
CRect rcValueBlock(125, 160, 480, 240);

CRect rcAbout(400, 6, 430, 22);

//button rect
#define BTN_Y_OFFSET 27
CRect rcBtnChangeMAC  (395, 230, 487, 254);
CRect rcBtnGenerateMAC(137, 267, 236, 282);

CRect rcBtnMinimize(440, 1, 440+29, 1+20);

//array for win7 wireless random mac
TCHAR g_chRandomSecondNibble[] = {'2', '6', 'A', 'E'};
#define NIBBLE_ARRAY_START 0
#define NIBBLE_ARRAY_END   3

CIPMACManagerDlg *g_pIPMACManagerDlg = NULL;

//TCHAR *chConnectionInfo[] = {_T("IPAddress"),     _T("SubnetMask"),     _T("DefaultGateway"),     _T("NameServer"),
//                            _T("DhcpIPAddress"),  _T("DhcpSubnetMask"), _T("DhcpDefaultGateway"), _T("DhcpNameServer")};

//m_arrAdapterData
//0 - Adapter Name
//1 - Connection Name
//2 - MAC
//3 - GUID
//4 - Spoof Status

//5 - Enable DHCP
//6 - IP
//7 - Subnet
//8 - Gateway
//9 - DNS

CIPMACManagerDlg::CIPMACManagerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CIPMACManagerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_eActiveTab = TAB_IP;
  m_cstrDisplayData = _T("");
  m_cstrDisplayValue = _T("");
  m_cstrCurrWriteTime = _T("");
  m_cstrLastBackup = _T("");

  m_pBtnSettingsChange = NULL;
  m_pBtnSettingsRestore = NULL;
  m_pBtnMACGenerate = NULL;
  m_pBtnClose = NULL;
  m_pBtnMinimize = NULL;

  m_iCurrentConnectionIndex = -1;
  m_bIsWindows7OrHigher = false;
  m_bWarnWirelessMACChange = false;
  m_bCancelNewCompatibleMAC = false;

  m_pUtilityDlg = NULL;
  m_pConfigNameDlg = NULL;
}

void CIPMACManagerDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_LIST_CONN, mListConnections);
  DDX_Control(pDX, IDC_EDIT1, mNewMAC);
}

BEGIN_MESSAGE_MAP(CIPMACManagerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_LBN_SELCHANGE(IDC_LIST_CONN, &CIPMACManagerDlg::OnLbnSelchangeListConn)
  ON_BN_CLICKED(BTN_ID_MAC_CHANGE, OnSettingsChangeButtonClick)
  ON_BN_CLICKED(BTN_ID_MAC_RESTORE, OnSettingsRestoreButtonClick)
  ON_BN_CLICKED(BTN_ID_MAC_GENERATE, OnMACGenerateButtonClick)
  ON_BN_CLICKED(BTN_ID_MINIMIZE, OnMinimizeButtonClick)
  ON_BN_CLICKED(BTN_ID_CLOSE, OnCloseButtonClick)
END_MESSAGE_MAP()


// CIPMACManagerDlg message handlers

BOOL CIPMACManagerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);

  g_pIPMACManagerDlg = this;
  SetWindowText(_T("IP MAC Manager"));
  ModifyStyle(0, WS_SYSMENU | WS_MINIMIZEBOX);
  
  GetOSVersion();
  
	RetrieveAdaptersInfo();
  
  LoadImagesFromResource();
  CreateRequiredFonts();
  InitializeControls();
  //SetControlsState();
  //SetFirstSelectionForMAC();

	return TRUE;
}

BOOL CIPMACManagerDlg::PreTranslateMessage(MSG *pMsg)
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
void CIPMACManagerDlg::OnPaint()
{
	CPaintDC dc(this);
  CDC pdc;
  pdc.CreateCompatibleDC(&dc);
  CBitmap *oldBmp = pdc.SelectObject(&m_BmpImg[0]);
  dc.BitBlt(0, 0, m_SizBmpImg[0].cx, m_SizBmpImg[0].cy, &pdc, 0, 0, SRCCOPY);
  dc.SelectObject(&oldBmp);

  dc.SetBkMode(TRANSPARENT);

  dc.SelectObject(&m_txtFont);
  //dc.SelectObject(&m_txtHeadingFont);
  dc.SetTextColor(RGB(255, 255, 255));
  //dc.FillSolidRect(rcAbout, RGB(0, 0, 0));
  CString cstrTopText = _T("About");
  dc.DrawText(cstrTopText, cstrTopText.GetLength(), rcAbout, DT_LEFT);

  dc.SetTextColor(RGB(223, 64, 2));
  dc.SelectObject(&m_txtHeadingFont);
  CString cstr = _T("Choose Connection");
  CRect rcConnection(15, 45, 125, 65);
  //dc.FillSolidRect(rcConnection, RGB(0,0,0));
  dc.DrawText(cstr, cstr.GetLength(), rcConnection, DT_LEFT);

  cstr = _T("IP Address");
  CRect rcIPAddress(16, 126, 80, 142);
  //dc.FillSolidRect(rcIPAddress, RGB(0,0,0));
  dc.DrawText(cstr, cstr.GetLength(), rcIPAddress, DT_LEFT);

  cstr = _T("MAC Address");
  CRect rcMACAddress(104, 126, 184, 142);
  //dc.FillSolidRect(rcMACAddress, RGB(0,0,0));
  dc.DrawText(cstr, cstr.GetLength(), rcMACAddress, DT_LEFT);

  dc.SelectObject(&m_txtFont);
  dc.SetTextColor(RGB(0, 0, 0));
  if(m_eActiveTab == TAB_MAC)
  {
    //tab draw
    oldBmp = pdc.SelectObject(&m_BmpImg[2]);
    dc.BitBlt(130, 141, m_SizBmpImg[2].cx, m_SizBmpImg[2].cy, &pdc, 0, 0, SRCCOPY);
    dc.SelectObject(&oldBmp);

    if(m_iCurrentConnectionIndex != -1)
    {
      //edit box draw
      oldBmp = pdc.SelectObject(&m_BmpImg[1]);
      dc.BitBlt(17, 259, m_SizBmpImg[1].cx, m_SizBmpImg[1].cy, &pdc, 0, 0, SRCCOPY);
      dc.SelectObject(&oldBmp);
    }
  }
  else
  {
    //tab draw
    oldBmp = pdc.SelectObject(&m_BmpImg[2]);
    dc.BitBlt(36, 141, m_SizBmpImg[2].cx, m_SizBmpImg[2].cy, &pdc, 0, 0, SRCCOPY);
    dc.SelectObject(&oldBmp);
  }

  if(m_iCurrentConnectionIndex != -1)
  {
    //data text
    //dc.FillSolidRect(rcDataBlock, RGB(220, 20, 20));
    dc.DrawText(m_cstrDisplayData, m_cstrDisplayData.GetLength(), rcDataBlock, DT_LEFT);

    //value text
    //dc.FillSolidRect(rcValueBlock, RGB(20, 220, 20));
    dc.DrawText(m_cstrDisplayValue, m_cstrDisplayValue.GetLength(), rcValueBlock, DT_LEFT);

    if(m_eActiveTab == TAB_MAC)
    {
      CString cstr = _T("New MAC Address");
      //dc.FillSolidRect(rcNewMACBlock, RGB(220, 20, 20));
      dc.DrawText(cstr, cstr.GetLength(), rcNewMACBlock, DT_LEFT);
    }
    else
    {
      CString cstr;
      if(m_cstrLastBackup.IsEmpty() == true)
        cstr = _T("No backup created");
      else
        cstr = _T("Last backup                 ") + m_cstrLastBackup;
      
      //dc.FillSolidRect(rcLastConfig, RGB(220, 20, 20));
      dc.DrawText(cstr, cstr.GetLength(), rcLastConfig, DT_LEFT | DT_END_ELLIPSIS);
    }
  }
  else
  {
    CRect rcValueBlock(140, 210, 360, 226);
    //dc.FillSolidRect(rcValueBlock, RGB(220, 20, 20));
    dc.SetTextColor(RGB(220, 20, 20));
    CString cstr = _T("Please choose preferred Connection");
    dc.DrawText(cstr, cstr.GetLength(), rcValueBlock, DT_CENTER);
  }


	//CDialog::OnPaint();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CIPMACManagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CIPMACManagerDlg::RetrieveAdaptersInfo(bool bRefresh, CString cstrAdapter)
{
  DWORD dwSize = 0;
  DWORD dwRetVal = 0;

  unsigned int i = 0;

  // Set the flags to pass to GetAdaptersAddresses
  ULONG flags = GAA_FLAG_INCLUDE_PREFIX;

  // default to unspecified address family (both)
  ULONG family = AF_INET;

  LPVOID lpMsgBuf = NULL;

  PIP_ADAPTER_ADDRESSES pAddresses = NULL;
  ULONG outBufLen = 0;

  PIP_ADAPTER_ADDRESSES pCurrAddresses = NULL;

  outBufLen = sizeof (IP_ADAPTER_ADDRESSES);
  pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);

  // Make an initial call to GetAdaptersAddresses to get the 
  // size needed into the outBufLen variable
  if (GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW) 
  {
    FREE(pAddresses);
    pAddresses = (IP_ADAPTER_ADDRESSES *) MALLOC(outBufLen);
  }

  if (pAddresses == NULL) 
  {
    AfxMessageBox(_T("Memory allocation failed for IP_ADAPTER_ADDRESSES struct"));
    return;
  }
  // Make a second call to GetAdapters Addresses to get the actual data we want

  dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

  if (dwRetVal == NO_ERROR) 
  {
    // If successful, output some information from the data we received
    pCurrAddresses = pAddresses;
    //CString cstrInfoText;
    short iAdapterCount = 0;

    while (pCurrAddresses) 
    {
      CString cstr = CString(pCurrAddresses->Description);
      if(bRefresh == true)
      {
        if(cstr != cstrAdapter)
        {
          pCurrAddresses = pCurrAddresses->Next;
          continue;
        }
      }

      if(cstr.Find(_T("Loopback")) != -1)
      {
        pCurrAddresses = pCurrAddresses->Next;
        continue;
      }

      m_LogWrite.LogEvent(_T("Adapter index: "), iAdapterCount);

      if(bRefresh == false)
      {
        //Description: Realtek RTL...
        m_arrAdapterData[iAdapterCount].Add(cstr);

        //FriendlyName: Local Area Connection
        cstr = CString(pCurrAddresses->FriendlyName);
        m_arrAdapterData[iAdapterCount].Add(cstr);
        mListConnections.AddString(cstr);

        m_LogWrite.LogEvent(_T("FriendlyName: ")+cstr);
      }

      //PhysicalAddress: 00-19-82-8A-6E
      if (pCurrAddresses->PhysicalAddressLength != 0) 
      {
        CString cstrMAC;
        for (i = 0; i < pCurrAddresses->PhysicalAddressLength; i++) 
        {
          CString cstrChar;
          if (i == (pCurrAddresses->PhysicalAddressLength - 1))
            cstrChar.Format(_T("%.2X"), (int)pCurrAddresses->PhysicalAddress[i]);
          else
            cstrChar.Format(_T("%.2X-"), (int)pCurrAddresses->PhysicalAddress[i]);

          cstrMAC += cstrChar;
        }

        if(bRefresh == false)
          m_arrAdapterData[iAdapterCount].Add(cstrMAC);
        else
          m_arrAdapterData[m_iCurrentConnectionIndex].SetAt(2, cstrMAC);

        m_LogWrite.LogEvent(_T("MAC Address: ")+cstrMAC);
      }

      cstr = CString(pCurrAddresses->AdapterName);
      if(bRefresh == false)
      {
        //AdapterName: {GUID}
        m_arrAdapterData[iAdapterCount].Add(cstr);
        m_LogWrite.LogEvent(_T("GUID: ")+cstr);
      }

      if(CheckMACSpoofed(cstr) == true)
      {
        if(bRefresh == false)
          m_arrAdapterData[iAdapterCount].Add(_T("Spoofed"));
        else
          m_arrAdapterData[m_iCurrentConnectionIndex].SetAt(4, _T("Spoofed"));
      }
      else
      {
        if(bRefresh == false)
          m_arrAdapterData[iAdapterCount].Add(_T("Not Spoofed"));
        else
          m_arrAdapterData[m_iCurrentConnectionIndex].SetAt(4, _T("Not Spoofed"));
      }

      //get IP info
      RetrieveConnectionInfo(iAdapterCount, cstr, bRefresh);

      pCurrAddresses = pCurrAddresses->Next;
      iAdapterCount++;
    }

    //DEBUG CODE
    //CString cstrData;
    //for(int i=0; i<10; i++)
    //{
    //  INT_PTR iSiz = m_arrAdapterData[i].GetCount();
    //  if(iSiz <= 0)
    //    continue;
    //  
    //  cstrData.Empty();
    //  for(int j=0; j<iSiz; j++)
    //  {
    //    CString cstr = m_arrAdapterData[i].GetAt(j);
    //    cstrData += cstr + _T("\r\n");
    //  }
    //  AfxMessageBox(cstrData);
    //}
  } 
  else 
  {
    CString cstr;
    cstr.Format(_T("Call to GetAdaptersAddresses failed with error: %d"), dwRetVal);
    AfxMessageBox(cstr);

    if (dwRetVal == ERROR_NO_DATA)
    {
      AfxMessageBox(_T("No addresses were found for the requested parameters"));
    }
    else 
    {
      if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwRetVal, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),   // Default language
        (LPTSTR) & lpMsgBuf, 0, NULL)) 
      {
        cstr.Format(_T("Error: %s"), lpMsgBuf);
        AfxMessageBox(cstr);
        LocalFree(lpMsgBuf);
        FREE(pAddresses);
        return;
      }
    }
  }
  FREE(pAddresses);
}

void CIPMACManagerDlg::LoadImagesFromResource()
{
  BITMAP bm = {0};
  //Back img
  m_BmpImg[0].LoadBitmap(IDB_BITMAP1);
  m_BmpImg[0].GetBitmap(&bm);
  m_SizBmpImg[0] = CSize(bm.bmWidth, bm.bmHeight);
  SetWindowPos(NULL, 0, 0, bm.bmWidth, bm.bmHeight, SWP_NOMOVE);

  m_BmpImg[1].LoadBitmap(IDB_BITMAP2);
  m_BmpImg[1].GetBitmap(&bm);
  m_SizBmpImg[1] = CSize(bm.bmWidth, bm.bmHeight);

  m_BmpImg[2].LoadBitmap(IDB_BITMAP3);
  m_BmpImg[2].GetBitmap(&bm);
  m_SizBmpImg[2] = CSize(bm.bmWidth, bm.bmHeight);
}

void CIPMACManagerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
  //CString cstr;
  //cstr.Format(_T("X: %d, Y: %d"), point.x, point.y);
  //AfxMessageBox(cstr);

  if((point.x >= 0 && point.x <= (rcAbout.left - 2)) && (point.y >= 0 && point.y <= 20))
  {
    PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
  }

  CDialog::OnLButtonDown(nFlags, point);
}

void CIPMACManagerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
  if((point.x >= rcAbout.left) && (point.x <= rcAbout.right) && (point.y >= rcAbout.top) && (point.y <= rcAbout.bottom))
  {
    CString cstAbout = _T("IP MAC Manager v1.5 \r\nDeveloped by hypheni \r\nMail: hyphen.on@gmail.com");
    AfxMessageBox(cstAbout, MB_ICONINFORMATION);
  }

  if(m_iCurrentConnectionIndex == -1)
  {
    return;
  }

  if((point.x >= rcTabIP.left) && (point.x <= rcTabIP.right) && (point.y >= rcTabIP.top) && (point.y <= rcTabIP.bottom))
  {
    OpenIPTab();
  }
  else if((point.x >= rcTabMAC.left) && (point.x <= rcTabMAC.right) && (point.y >= rcTabMAC.top) && (point.y <= rcTabMAC.bottom))
  {
    OpenMACTab();
  }

  CDialog::OnLButtonUp(nFlags, point);
}

LRESULT CIPMACManagerDlg::OnNcHitTest(UINT uParam, LONG lParam)
{
  int xPos = LOWORD(lParam);
  int yPos = HIWORD(lParam);
  UINT nHitTest = CDialog::OnNcHitTest(CSize(xPos, yPos));

  return (nHitTest == HTCLIENT) ? HTCAPTION : nHitTest;
}

void CIPMACManagerDlg::OnLbnSelchangeListConn()
{
  int iIndex = mListConnections.GetCurSel();
  if(iIndex != m_iCurrentConnectionIndex)
  {
    m_iCurrentConnectionIndex = iIndex;
  }
  else
  {
    return;
  }

  if(m_iCurrentConnectionIndex != -1)
  {
    SetControlsState();
    ConnectionSelectionChange();
  }
}

void CIPMACManagerDlg::InitializeControls()
{
  //Hide edit control
  mNewMAC.ShowWindow(FALSE);

  //Button - Change
  m_pBtnSettingsChange = new CButtonST;
  if(m_pBtnSettingsChange != NULL)
  {
    m_pBtnSettingsChange->Create(_T("Backup"), WS_CHILD | WS_GROUP | WS_TABSTOP, rcBtnChangeMAC, this, BTN_ID_MAC_CHANGE);
    m_pBtnSettingsChange->SetFont(&m_txtFont);
    m_pBtnSettingsChange->DrawBorder(FALSE);

    m_pBtnSettingsChange->SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(197, 59, 5));
    m_pBtnSettingsChange->SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(223, 64, 2));
    m_pBtnSettingsChange->SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(223, 64, 2));

    m_pBtnSettingsChange->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
    m_pBtnSettingsChange->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(255, 255, 255));
    m_pBtnSettingsChange->SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255));
  }

  //Button - Restore
  m_pBtnSettingsRestore = new CButtonST;
  if(m_pBtnSettingsRestore != NULL)
  {
    CRect rc(rcBtnChangeMAC);
    rc.top    += BTN_Y_OFFSET;
    rc.bottom += BTN_Y_OFFSET;
    m_pBtnSettingsRestore->Create(_T("Restore"), WS_CHILD | WS_GROUP | WS_TABSTOP, rc, this, BTN_ID_MAC_RESTORE);
    m_pBtnSettingsRestore->SetFont(&m_txtFont);
    m_pBtnSettingsRestore->DrawBorder(FALSE);

    m_pBtnSettingsRestore->SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(197, 59, 5));
    m_pBtnSettingsRestore->SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(223, 64, 2));
    m_pBtnSettingsRestore->SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(223, 64, 2));

    m_pBtnSettingsRestore->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
    m_pBtnSettingsRestore->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(255, 255, 255));
    m_pBtnSettingsRestore->SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255));
  }

  //Button - MAC Generate
  m_pBtnMACGenerate = new CButtonST;
  if(m_pBtnMACGenerate != NULL)
  {
    m_pBtnMACGenerate->Create(_T("Generate Random"), WS_CHILD | WS_GROUP | WS_TABSTOP, rcBtnGenerateMAC, this, BTN_ID_MAC_GENERATE);
    m_pBtnMACGenerate->SetFont(&m_txtFont);
    
    m_pBtnMACGenerate->DrawTransparent();
    m_pBtnMACGenerate->DrawBorder(FALSE);
    m_pBtnMACGenerate->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(223, 64, 2));
    m_pBtnMACGenerate->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(223, 64, 2));
    m_pBtnMACGenerate->SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(223, 64, 2));
  }

  //Button - Minimize
  m_pBtnMinimize = new CButtonST;
  if(m_pBtnMinimize != NULL)
  {
    m_pBtnMinimize->Create(_T(""), WS_CHILD | WS_GROUP | WS_TABSTOP | WS_VISIBLE, rcBtnMinimize, this, BTN_ID_MINIMIZE);
    m_pBtnMinimize->DrawBorder(FALSE);
    m_pBtnMinimize->SetBitmaps(IDB_BITMAP9, 0, IDB_BITMAP8, 0);
  }

  //Button - Close
  m_pBtnClose = new CButtonST;
  if(m_pBtnClose != NULL)
  {
    CRect rcTemp(rcBtnMinimize);
    rcTemp.left = rcBtnMinimize.right + 1;
    rcTemp.right = rcBtnMinimize.right + 29;

    m_pBtnClose->Create(_T(""), WS_CHILD | WS_GROUP | WS_TABSTOP | WS_VISIBLE, rcTemp, this, BTN_ID_CLOSE);
    m_pBtnClose->DrawBorder(FALSE);
    m_pBtnClose->SetBitmaps(IDB_BITMAP7, 0, IDB_BITMAP6, 0);
  }
}

void CIPMACManagerDlg::CreateRequiredFonts()
{
  m_txtFont.CreateFont(14,			 // nHeight
    0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_NORMAL,								 // nWeight
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
    _T("Serif"));

  m_txtHeadingFont.CreateFont(14,// nHeight
    0,                         // nWidth
    0,                         // nEscapement
    0,                         // nOrientation
    FW_BOLD,								   // nWeight
    FALSE,                     // bItalic
    FALSE,                     // bUnderline
    0,                         // cStrikeOut
    ANSI_CHARSET,              // nCharSet
    OUT_DEFAULT_PRECIS,        // nOutPrecision
    CLIP_DEFAULT_PRECIS,       // nClipPrecision
    DEFAULT_QUALITY,           // nQuality
    DEFAULT_PITCH | FF_DONTCARE,  // nPitchAndFamily
    _T("Serif"));
}

void CIPMACManagerDlg::OnSettingsChangeButtonClick()
{
  if(m_eActiveTab == TAB_MAC)
  {
    OnMACSettingsChange();
  }
  else
  {
    OnIPSettingsChange();
  }
}

void CIPMACManagerDlg::OnSettingsRestoreButtonClick()
{
  if(m_eActiveTab == TAB_MAC)
  {
    OnMACSettingsRestore();
  }
  else
  {
    OnIPSettingsRestore();
  }
}

void CIPMACManagerDlg::OnMACGenerateButtonClick()
{
  CString cstrMAC = GenerateRandomMACAddress();
  mNewMAC.SetWindowText(cstrMAC);
}

void CIPMACManagerDlg::SetControlsState()
{
  if(m_eActiveTab == TAB_MAC)
  {
    //Edit
    if(::IsWindow(mNewMAC.GetSafeHwnd()) == TRUE)
    {
      if(mNewMAC.IsWindowVisible() == FALSE)
        mNewMAC.ShowWindow(TRUE);
    }

    //Btn - Change
    if((m_pBtnSettingsChange != NULL) && (::IsWindow(m_pBtnSettingsChange->GetSafeHwnd()) == TRUE))
    {
      m_pBtnSettingsChange->SetWindowText(_T("Change"));

      if(m_pBtnSettingsChange->IsWindowVisible() == FALSE)
        m_pBtnSettingsChange->ShowWindow(TRUE);
    }

    //Btn - Restore
    if((m_pBtnSettingsRestore != NULL) && (::IsWindow(m_pBtnSettingsRestore->GetSafeHwnd()) == TRUE))
    {
      if(m_pBtnSettingsRestore->IsWindowVisible() == FALSE)
        m_pBtnSettingsRestore->ShowWindow(TRUE);
    }

    //Btn - Generate
    if((m_pBtnMACGenerate != NULL) && (::IsWindow(m_pBtnMACGenerate->GetSafeHwnd()) == TRUE))
    {
      if(m_pBtnMACGenerate->IsWindowVisible() == FALSE)
        m_pBtnMACGenerate->ShowWindow(TRUE);
    }
  }
  else
  {
    //Edit
    if(::IsWindow(mNewMAC.GetSafeHwnd()) == TRUE)
    {
      if(mNewMAC.IsWindowVisible() == TRUE)
        mNewMAC.ShowWindow(FALSE);
    }

    //Btn - Backup
    if((m_pBtnSettingsChange != NULL) && (::IsWindow(m_pBtnSettingsChange->GetSafeHwnd()) == TRUE))
    {
      m_pBtnSettingsChange->SetWindowText(_T("Backup"));

      if(m_pBtnSettingsChange->IsWindowVisible() == FALSE)
        m_pBtnSettingsChange->ShowWindow(TRUE);
    }

    //Btn - Restore
    if((m_pBtnSettingsRestore != NULL) && (::IsWindow(m_pBtnSettingsRestore->GetSafeHwnd()) == TRUE))
    {
      if(m_pBtnSettingsRestore->IsWindowVisible() == FALSE)
        m_pBtnSettingsRestore->ShowWindow(TRUE);
    }

    //Btn - Generate
    if((m_pBtnMACGenerate != NULL) && (::IsWindow(m_pBtnMACGenerate->GetSafeHwnd()) == TRUE))
    {
      if(m_pBtnMACGenerate->IsWindowVisible() == TRUE)
        m_pBtnMACGenerate->ShowWindow(FALSE);
    }
  }
}

void CIPMACManagerDlg::RestartAdapter(CString cstrAdapter)
{
	INetConnectionManager* pNet;
	INetConnection* pConn;
	IEnumNetConnection* pEnum;
	NETCON_PROPERTIES* pProps;
	ULONG uCount = 0;

	CoInitialize(NULL);
	CoCreateInstance(CLSID_ConnectionManager, NULL, CLSCTX_SERVER, IID_INetConnectionManager, (void**)&pNet);
	pNet->EnumConnections(NCME_DEFAULT, &pEnum);

	while(pEnum->Next(1, &pConn, &uCount) == S_OK)
	{
		pConn->GetProperties(&pProps);
    if(cstrAdapter.Compare(pProps->pszwDeviceName) == 0)
		{
			pConn->Disconnect();
      Sleep(200);
			pConn->Connect();
			//CoTaskMemFree(pProps->pszwName);
			//CoTaskMemFree(pProps->pszwDeviceName);
			CoTaskMemFree(pProps);
			pConn->Release();
		}
	}
	pEnum->Release();
	pNet->Release();
	CoUninitialize();
}

void CIPMACManagerDlg::ChangeMACAddress(CString csNewMACAdd)
{
  if(m_arrAdapterData[m_iCurrentConnectionIndex].GetCount() <= 0)
  {
    AfxMessageBox(_T("No data found to be applied"));
    return;
  }

  if(AfxMessageBox(_T("Connectivity of your selected connection will be lost for few seconds during MAC Address change. Do you want to continue ?"), MB_YESNO) == IDNO)
  {
    return;
  }
  csNewMACAdd.Remove('-');

	DWORD BufSiz = 200;
  HKEY hKeyAdapterID = NULL;
	TCHAR szKeyName[200] = _T("");
  int iIndex = 0;
  TCHAR szMACAdd[200] = _T("");
  _tcscpy_s(szMACAdd, 200, (LPCTSTR)csNewMACAdd);
  CString cstrSelectionGUID = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(3);
  CString cstrSelectionAdapter = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(0);

  RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}"), 0, KEY_READ, &hKeyAdapterID);
	while(RegEnumKeyEx(hKeyAdapterID, iIndex++, szKeyName, &BufSiz, 0, NULL, NULL, NULL) == ERROR_SUCCESS) 
	{
    HKEY hKeyQuery = NULL;
    BufSiz = 200;
    TCHAR szNetCardGUID[200] = _T("");
		RegOpenKeyEx(hKeyAdapterID, szKeyName, 0, KEY_READ | KEY_WRITE, &hKeyQuery);
		RegQueryValueEx(hKeyQuery, _T("NetCfgInstanceId"), 0, NULL, (LPBYTE)szNetCardGUID, &BufSiz);
		
    if(cstrSelectionGUID.Compare(szNetCardGUID) == 0)
		{
      DWORD dwSiz = (DWORD)(sizeof(TCHAR) * (_tcslen(szMACAdd) + 1));
			LSTATUS iReturn = RegSetValueEx(hKeyQuery, _T("NetworkAddress"), 0, REG_SZ, (const BYTE*)szMACAdd, dwSiz);
      if(iReturn == ERROR_SUCCESS)
			{
        RegCloseKey(hKeyQuery);
				RestartAdapter(cstrSelectionAdapter);
        Sleep(200);
        RefreshAdapterInfo();
				AfxMessageBox(_T("MAC Address spoofed successfully done"));
        ConnectionSelectionChange();
        break;
			}
			else
      {
        AfxMessageBox(_T("Could not set new MAC Address. If this continues contact product vendor."));
      }
		}
		
	}
	RegCloseKey (hKeyAdapterID);
}

bool CIPMACManagerDlg::CheckMACSpoofed(CString cstrGUID)
{
  bool bReturn = false;

	DWORD dwBufSiz = 200;
  HKEY hKeyAdapterID = NULL;
	TCHAR szKeyName[200] = _T("");
  int iIndex = 0;
  CString cstrSelectionGUID;
  if(cstrGUID.IsEmpty() == true)
    cstrSelectionGUID = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(3);
  else
    cstrSelectionGUID = cstrGUID;

  RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}"), 0, KEY_READ, &hKeyAdapterID);
	while(RegEnumKeyEx(hKeyAdapterID, iIndex++, szKeyName, &dwBufSiz, 0, NULL, NULL, NULL) == ERROR_SUCCESS) 
	{
    HKEY hKeyQuery = NULL;
    dwBufSiz = 200;
    TCHAR szNetCardGUID[200] = _T("");
		RegOpenKeyEx(hKeyAdapterID, szKeyName, 0, KEY_READ | KEY_WRITE, &hKeyQuery);
		RegQueryValueEx(hKeyQuery, _T("NetCfgInstanceId"), 0, NULL, (LPBYTE)szNetCardGUID, &dwBufSiz);

    if(cstrSelectionGUID.Compare(szNetCardGUID) == 0)
		{
      TCHAR szMACAddress[200] = _T("");
      dwBufSiz = 200;
      if(RegQueryValueEx(hKeyQuery, _T("NetworkAddress"), 0, NULL, (LPBYTE)szMACAddress, &dwBufSiz) == ERROR_SUCCESS)
      {
        CString cstrTemp(szMACAddress);
        if(cstrTemp.IsEmpty() == false)
        {
          bReturn = true;
        }
      }
      else
      {
        bReturn = false;
      }
      break;
    }

    RegCloseKey(hKeyQuery);
  }
  RegCloseKey(hKeyAdapterID);

  return bReturn;
}

void CIPMACManagerDlg::RestoreMACAddress()
{
	DWORD dwBufSiz = 200;
  HKEY hKeyAdapterID = NULL;
	TCHAR szKeyName[200] = _T("");
  int iIndex = 0;
  CString cstrSelectionGUID = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(3);
  CString cstrSelectionAdapter = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(0);

  RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}"), 0, KEY_READ, &hKeyAdapterID);
	while(RegEnumKeyEx(hKeyAdapterID, iIndex++, szKeyName, &dwBufSiz, 0, NULL, NULL, NULL) == ERROR_SUCCESS) 
	{
    HKEY hKeyQuery = NULL;
    dwBufSiz = 200;
    TCHAR szNetCardGUID[200] = _T("");
		RegOpenKeyEx(hKeyAdapterID, szKeyName, 0, KEY_READ | KEY_WRITE, &hKeyQuery);
		RegQueryValueEx(hKeyQuery, _T("NetCfgInstanceId"), 0, NULL, (LPBYTE)szNetCardGUID, &dwBufSiz);

    if(cstrSelectionGUID.Compare(szNetCardGUID) == 0)
		{
      LSTATUS iReturn = RegDeleteValue(hKeyQuery, _T("NetworkAddress"));
      if(iReturn == ERROR_SUCCESS)
      {
        RegCloseKey (hKeyQuery);
        RestartAdapter(cstrSelectionAdapter);
        Sleep(200);
        RefreshAdapterInfo();
        AfxMessageBox(_T("Successfully restored MAC Address to its original"));
        ConnectionSelectionChange();
        break;
      }
    }

    RegCloseKey(hKeyQuery);
  }
  RegCloseKey(hKeyAdapterID);
}

void CIPMACManagerDlg::RefreshAdapterInfo(bool bCalledFromOnInit)
{
  CString cstrAdapter = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(0);
  m_LogWrite.LogEvent(_T("Going to refresh adapter info for: ")+cstrAdapter);
  RetrieveAdaptersInfo(true, cstrAdapter);
}

void CIPMACManagerDlg::SetFirstSelectionForMAC()
{
  m_iCurrentConnectionIndex = 0;
  mListConnections.SetCurSel(m_iCurrentConnectionIndex);
  RefreshAdapterInfo(true);
}

void CIPMACManagerDlg::GetOSVersion()
{
  DWORD dwVersion = GetVersion();
  DWORD dwOSMajorVer = (DWORD)(LOBYTE(LOWORD(dwVersion)));
  DWORD dwOSMinorVer = (DWORD)(HIBYTE(LOWORD(dwVersion)));

  if(dwOSMajorVer >= 6 && dwOSMinorVer >= 1)
    m_bIsWindows7OrHigher = true;
}

CString CIPMACManagerDlg::GenerateRandomMACAddress()
{
  //Sample hex random number generate
  //cstr.Format(_T("%.2X-%.2X-%.2X-%.2X-%.2X-%.2X"), rand() & 0xFF, rand() & 0xFF, rand() & 0xFF, rand() & 0xFF, rand() & 0xFF, rand() & 0xFF);
	
  CString cstrMAC;

  //Check whether OS >= Win7 and Network adapter is a Wireless card
  if((m_bIsWindows7OrHigher == true) && (m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(0).Find(_T("Wireless")) != -1))
  {
    //formula - rand number in range
    //(rand() % (max + 1 - min)) + min

    short iNibble = (rand() % (NIBBLE_ARRAY_END + 1 - NIBBLE_ARRAY_START)) + NIBBLE_ARRAY_START;
    cstrMAC.Format(_T("0%c-"), g_chRandomSecondNibble[iNibble]);
  }
  else
  {
    cstrMAC = _T("00-");
  }

  CString cstrTemp;
  int iNo = 0;
  for(int i=0; i<5 && (iNo = rand()%32) > -1; i++) 
  {
    CString cstr;
    cstr.Format(_T("%s%X%s"), iNo<16 ? "0" : "", iNo, i<4 ? "-" : "");
    cstrTemp += cstr;
  }
  cstrMAC += cstrTemp;
  
  return cstrMAC;
}

//ip manage
void CIPMACManagerDlg::RetrieveConnectionInfo(short iAdapterIndex, CString cstrAdapterGUID, bool bRefresh)
{
  CString cstrKey = _T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\");
  cstrKey += cstrAdapterGUID;

  HKEY hKey = NULL;
  LSTATUS iReturn = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCTSTR)cstrKey, 0, KEY_READ, &hKey);
  if(iReturn == ERROR_SUCCESS)
  {
    CString cstrValue;
    DWORD dwBufSiz(sizeof(DWORD));
    DWORD dwValue(0);
    iReturn = RegQueryValueEx(hKey, _T("EnableDHCP"), 0, NULL, (LPBYTE)&dwValue, &dwBufSiz);
    if(iReturn != ERROR_SUCCESS)
      m_LogWrite.LogEvent(_T("Failed RetrieveConnectionInfo Reason: RegQueryValueEx Key: EnableDHCP"), iReturn);
    else
      m_LogWrite.LogEvent(_T("Adapter EnableDHCP: "), dwValue);

    cstrValue.Format(_T("%d"), dwValue);
    if(bRefresh == false)
      m_arrAdapterData[iAdapterIndex].Add(cstrValue);
    else
      m_arrAdapterData[iAdapterIndex].SetAt(5, cstrValue);

    CString cstrQueryIP, cstrQuerySubnet, cstrQueryGateway, cstrQueryNameServer;
    if(dwValue == 0)
    {
      cstrQueryIP         = _T("IPAddress");
      cstrQuerySubnet     = _T("SubnetMask");
      cstrQueryGateway    = _T("DefaultGateway");
      cstrQueryNameServer = _T("NameServer");
    }
    else
    {
      cstrQueryIP         = _T("DhcpIPAddress");
      cstrQuerySubnet     = _T("DhcpSubnetMask");
      cstrQueryGateway    = _T("DhcpDefaultGateway");
      cstrQueryNameServer = _T("DhcpNameServer");
    }

    //IP
    TCHAR szBuff[200] = _T("");
    dwBufSiz = 400;
    iReturn = RegQueryValueEx(hKey, cstrQueryIP, 0, NULL, (LPBYTE)&szBuff, &dwBufSiz);
    if(iReturn != ERROR_SUCCESS)
      m_LogWrite.LogEvent(_T("Failed RetrieveConnectionInfo Reason: RegQueryValueEx Key: ")+cstrQueryIP, iReturn);
    else
      m_LogWrite.LogEvent(_T("Adapter ")+cstrQueryIP + _T(" ") + szBuff);

    if(bRefresh == false)
      m_arrAdapterData[iAdapterIndex].Add(szBuff);
    else
      m_arrAdapterData[iAdapterIndex].SetAt(6, szBuff);

    //Subnet
    ZeroMemory(szBuff, sizeof(szBuff));
    dwBufSiz = 400;
    iReturn = RegQueryValueEx(hKey, cstrQuerySubnet, 0, NULL, (LPBYTE)&szBuff, &dwBufSiz);
    if(iReturn != ERROR_SUCCESS)
      m_LogWrite.LogEvent(_T("Failed RetrieveConnectionInfo Reason: RegQueryValueEx Key: ")+cstrQuerySubnet, iReturn);
    else
      m_LogWrite.LogEvent(_T("Adapter ")+cstrQuerySubnet + _T(" ") + szBuff);

    if(bRefresh == false)
      m_arrAdapterData[iAdapterIndex].Add(szBuff);
    else
      m_arrAdapterData[iAdapterIndex].SetAt(7, szBuff);

    //Gateway
    ZeroMemory(szBuff, sizeof(szBuff));
    dwBufSiz = 400;
    iReturn = RegQueryValueEx(hKey, cstrQueryGateway, 0, NULL, (LPBYTE)&szBuff, &dwBufSiz);
    if(iReturn != ERROR_SUCCESS)
      m_LogWrite.LogEvent(_T("Failed RetrieveConnectionInfo Reason: RegQueryValueEx Key: ")+cstrQueryGateway, iReturn);
    else
      m_LogWrite.LogEvent(_T("Adapter ")+cstrQueryGateway + _T(" ") + szBuff);

    if(bRefresh == false)
      m_arrAdapterData[iAdapterIndex].Add(szBuff);
    else
      m_arrAdapterData[iAdapterIndex].SetAt(8, szBuff);

    //NameServer
    ZeroMemory(szBuff, sizeof(szBuff));
    dwBufSiz = 400;
    iReturn = RegQueryValueEx(hKey, cstrQueryNameServer, 0, NULL, (LPBYTE)&szBuff, &dwBufSiz);
    if(iReturn != ERROR_SUCCESS)
      m_LogWrite.LogEvent(_T("Failed RetrieveConnectionInfo Reason: RegQueryValueEx Key: ")+cstrQueryNameServer, iReturn);
    else
      m_LogWrite.LogEvent(_T("Adapter ")+cstrQueryNameServer + _T(" ") + szBuff);

    if(bRefresh == false)
      m_arrAdapterData[iAdapterIndex].Add(szBuff);
    else
      m_arrAdapterData[iAdapterIndex].SetAt(9, szBuff);
  }
  else
  {
    m_LogWrite.LogEvent(_T("Failed RetrieveConnectionInfo Reason: RegOpenKeyEx Key: ")+cstrKey, iReturn);
  }
}

void CIPMACManagerDlg::ConnectionSelectionChange()
{
  if(m_eActiveTab == TAB_MAC)
  {
    CString cstrAdapter = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(0);
    CString cstrMAC     = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(2);
    CString cstrSpoofed = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(4);

    m_cstrDisplayData  = _T("Adapter Name")     + CString(_T("\r\n")) +
                         _T("MAC Address")      + CString(_T("\r\n")) +
                         _T("Status")           + CString(_T("\r\n"));

    m_cstrDisplayValue = cstrAdapter  + _T("\r\n") +
                         cstrMAC      + _T("\r\n") +
                         cstrSpoofed  + _T("\r\n");
  }
  else
  {
    CString cstrDHCP    = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(5);
    CString cstrIP      = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(6);
    CString cstrSubnet  = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(7);
    CString cstrGateway = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(8);
    CString cstrDNS     = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(9);
    //cstrDNS.Replace(',', '\n');

    (cstrDHCP == _T("1")) ? cstrDHCP = _T("True") : cstrDHCP = _T("False");

    m_cstrDisplayData  = _T("DHCP Enabled")     + CString(_T("\r\n")) +
                         _T("IP Address")       + CString(_T("\r\n")) +
                         _T("Subnet Mask")      + CString(_T("\r\n")) +
                         _T("Default Gateway")  + CString(_T("\r\n")) +
                         _T("DNS Server(s)")    + CString(_T("\r\n"));

    m_cstrDisplayValue = cstrDHCP    + _T("\r\n") +
                         cstrIP      + _T("\r\n") +
                         cstrSubnet  + _T("\r\n") +
                         cstrGateway + _T("\r\n") +
                         cstrDNS     + _T("\r\n");

    RefreshLastBackupArea(true);
  }

  InvalidateRect(rcDataBlock);
  InvalidateRect(rcValueBlock);
  InvalidateRect(rcRefresh);
  UpdateWindow();
}

void CIPMACManagerDlg::OnMACSettingsChange()
{
  CString cstr;
  mNewMAC.GetWindowText(cstr);
  if(IsValidMAC(cstr) == false)
  {
    AfxMessageBox(_T("Enter a valid MAC Address to continue"), MB_OK | MB_ICONEXCLAMATION);
    return;
  }

  //Wireless adapter mac change on >= windows 7
  if((m_bIsWindows7OrHigher == true) && (m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(0).Find(_T("Wireless")) != -1))
  {
    TCHAR szNibble = cstr.GetAt(1);
    bool bFound = false;
    for(short i = NIBBLE_ARRAY_START; i <= NIBBLE_ARRAY_END; i++)
    {
      if(szNibble == g_chRandomSecondNibble[i])
      {
        bFound = true;
        break;
      }
    }

    if(bFound == false)
    {
      CreateUtilityDialog();
      m_pUtilityDlg->SetMACAddress(cstr);
      m_pUtilityDlg->SetScreenParameters(WIRELESS_MAC);
      m_pUtilityDlg->ShowWindow(TRUE);
    }
    else
    {
      ChangeMACAddress(cstr);
    }
  }
  //Normal adapters mac change
  else
  {
    ChangeMACAddress(cstr);
  }
}

void CIPMACManagerDlg::OnMACSettingsRestore()
{
  if(CheckMACSpoofed() == false)
  {
    AfxMessageBox(_T("MAC Address has not been spoofed yet for this Adapter"));
    return;
  }
  
  if(AfxMessageBox(_T("Connectivity of your selected connection will be lost for few seconds during MAC Address restore. Do you want to continue ?"), MB_YESNO) == IDNO)
  {
    return;
  }

  RestoreMACAddress();
}

void CIPMACManagerDlg::OnIPSettingsChange()
{
  AskForConfigName();
}

void CIPMACManagerDlg::CreateIPBackup(CString cstrConfigName)
{
  CString cstrFile = BuildIPBackupFileName();
  
  //{{Reg hive save
  SetPrivilege(SE_BACKUP_NAME, TRUE);
  CString cstrKeyToSave  = _T("SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\");
  CString cstrSelectionGUID = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(3);
  cstrKeyToSave += cstrSelectionGUID;

  HKEY hKeyToSave = NULL;
  LSTATUS iStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE, cstrKeyToSave, 0, KEY_READ, &hKeyToSave);
  if(iStatus == ERROR_SUCCESS)
  {
    CString cstrHiveFile = cstrFile + _T("dat");
    RegSaveKey(hKeyToSave, (LPCTSTR)cstrHiveFile, 0);
    RegCloseKey(hKeyToSave);
  }
  SetPrivilege(SE_BACKUP_NAME, FALSE);
  //}}Reg hive save

  CString cstrGUID        = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(3);
  CString cstrBackupName  = cstrConfigName;
  CString cstrTime        = m_cstrCurrWriteTime;
  CString cstrDHCPEnable  = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(5);
  CString cstrIP          = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(6);
  CString cstrSubnet      = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(7);
  CString cstrGateway     = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(8);
  CString cstrDNS         = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(9);
  
  //{GUID}|Name|Time|DHCP|IP|Subnet|Gateway|DNS
  CString cstrTotalData = cstrGUID        + _T("|") + 
                          cstrBackupName  + _T("|") + 
                          cstrTime        + _T("|") + 
                          cstrDHCPEnable  + _T("|") + 
                          cstrIP          + _T("|") + 
                          cstrSubnet      + _T("|") + 
                          cstrGateway     + _T("|") + 
                          cstrDNS;

  CStdioFile oFile;
  CString cstrInfoFile = cstrFile + _T("ipm");
  BOOL bReturn = oFile.Open((LPCTSTR)cstrInfoFile, CFile::modeCreate | CFile::modeWrite);
  if(bReturn == TRUE)
  {
    oFile.WriteString(cstrTotalData);
    oFile.Close();
    WriteLastToRegistry(cstrTotalData);
    AfxMessageBox(_T("IP backup successfully created"), MB_OK|MB_ICONINFORMATION);
    RefreshLastBackupArea();
  }
  else
  {
    AfxMessageBox(_T("Failed to create IP backup!"), MB_OK|MB_ICONWARNING);
  }
}

void CIPMACManagerDlg::OnIPSettingsRestore()
{
  if(m_cstrLastBackup.IsEmpty() == true)
  {
    AfxMessageBox(_T("No backups has been created for this connection"));
    return;
  }

  CreateUtilityDialog();
  CString cstrGUID = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(3);
  m_pUtilityDlg->SetScreenParameters(IP_RESTORE, cstrGUID);
  m_pUtilityDlg->ShowWindow(TRUE);
}

void CIPMACManagerDlg::OpenIPTab()
{
  if(m_eActiveTab != TAB_IP)
  {
    m_eActiveTab = TAB_IP;

    SetControlsState();
    ConnectionSelectionChange();
  }
}

void CIPMACManagerDlg::OpenMACTab()
{
  if(m_eActiveTab != TAB_MAC)
  {
    m_eActiveTab = TAB_MAC;

    SetControlsState();
    ConnectionSelectionChange();
  }
}

CString CIPMACManagerDlg::BuildIPBackupFileName()
{
  TCHAR szCurrentPath[MAX_PATH] = _T("");
  ::GetCurrentDirectory(MAX_PATH, szCurrentPath);
  CString cstrBackupFolder(szCurrentPath);
  cstrBackupFolder += _T("\\Backups");
  ::CreateDirectory(cstrBackupFolder, NULL);

  CString cstrSelectionGUID = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(3);
  COleDateTime oDateTime = COleDateTime::GetCurrentTime();
  CString cstrTime;
  cstrTime.Format(_T("%02d/%02d/%04d %02d:%02d:%02d"), oDateTime.GetDay(), oDateTime.GetMonth(), oDateTime.GetYear(), 
                                                 oDateTime.GetHour(), oDateTime.GetMinute(), oDateTime.GetSecond());
  m_cstrCurrWriteTime = cstrTime;
  cstrTime.Remove('/');
  cstrTime.Remove(' ');
  cstrTime.Remove(':');

  int iPos = cstrSelectionGUID.Find(_T("-"));
  CString cstrFormattedGUID = cstrSelectionGUID.Mid(1, iPos-1);

  CString cstrBackupFile;
  cstrBackupFile.Format(_T("%s\\%s-%s."), cstrBackupFolder, cstrFormattedGUID, cstrTime);
  return cstrBackupFile;
}

bool CIPMACManagerDlg::IsValidMAC(CString cstrMAC)
{
  if(cstrMAC.IsEmpty() == true) 
    return false;

  cstrMAC.Remove('-');
  
  if(cstrMAC.GetLength() < 12)
    return false;

  for(short i = 0; i < 12; i++)
  {
    TCHAR chNibble = cstrMAC.GetAt(i);
    if ((chNibble < '0' || chNibble > '9') && (chNibble < 'A' || chNibble > 'F')) 
      return false;
  }

  return true;
}

void CIPMACManagerDlg::CreateUtilityDialog()
{
  if(m_pUtilityDlg == NULL)
  {
    m_pUtilityDlg = new CUtilityDlg();
    m_pUtilityDlg->Create(IDD_UTILITY_DIALOG);
  }
}

void CIPMACManagerDlg::AskForConfigName()
{
  if(m_pConfigNameDlg == NULL)
  {
    m_pConfigNameDlg = new CConfigNameDlg();
    m_pConfigNameDlg->Create(IDD_CONFIG_NAME);
  }

  m_pConfigNameDlg->Initialize();
}

void CIPMACManagerDlg::WriteLastToRegistry(CString cstrData)
{
  CString cstrGUID;
  AfxExtractSubString(cstrGUID, cstrData, 0, '|');
  CString ctrAdapterKey = _T("SOFTWARE\\IPMAC\\");
  ctrAdapterKey += cstrGUID;
  HKEY hKey = NULL;

  LSTATUS iReturn = RegCreateKeyEx(HKEY_LOCAL_MACHINE, (LPCTSTR)ctrAdapterKey, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, 0);
  if(iReturn == ERROR_SUCCESS)
  {
    CString cstrBackupName;
    CString cstrTime;
    CString cstrDataToStore;
    
    AfxExtractSubString(cstrBackupName, cstrData, 1, '|');
    AfxExtractSubString(cstrTime, cstrData, 2, '|');
    cstrDataToStore = cstrBackupName + _T(" (") + cstrTime + _T(")");

    TCHAR szMACAdd[200] = _T("");
    _tcscpy_s(szMACAdd, 200, (LPCTSTR)cstrDataToStore);
    DWORD dwSiz = (DWORD)(sizeof(TCHAR) * (_tcslen(szMACAdd) + 1));
    RegSetValueEx(hKey, _T("LastBackup"), 0, REG_SZ, (const BYTE*)szMACAdd, dwSiz);
    RegCloseKey(hKey);
  }
  else
  {
    AfxMessageBox(_T("Failed to save latest data to registry"));
  }
}

void CIPMACManagerDlg::ReadLastBackupData()
{
  HKEY hKey = NULL;
  CString cstrSelectionGUID = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(3);
  CString cstrKey = _T("SOFTWARE\\IPMAC\\");
  cstrKey += cstrSelectionGUID;
  m_cstrLastBackup.Empty();

  LSTATUS iReturn = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCTSTR)cstrKey, 0, KEY_READ, &hKey);
  if(iReturn == ERROR_SUCCESS)
  {
    TCHAR szBuff[200] = _T("");
    DWORD dwBufSiz = 400;
    RegQueryValueEx(hKey, _T("LastBackup"), 0, NULL, (LPBYTE)&szBuff, &dwBufSiz);
    m_cstrLastBackup = CString(szBuff);
    RegCloseKey(hKey);
  }
  else
  {
    //AfxMessageBox(_T("Failed to get latest data from registry"));
  }
}

void CIPMACManagerDlg::RefreshLastBackupArea(bool bOnlyInvalidate)
{
  ReadLastBackupData();
  InvalidateRect(rcLastConfig);
  if(bOnlyInvalidate == false)
    UpdateWindow(); 
}

void CIPMACManagerDlg::ChangeIPAddress(CString cstrFileName, CString cstrBackupData)
{
  //{GUID}|Name|Time|DHCP|IP|Subnet|Gateway|DNS

  CString cstrGUID, cstrDHCP; //, cstrIP, cstrSubnet, cstrGateway, cstrDNS;
  AfxExtractSubString(cstrGUID, cstrBackupData, 0, '|');
  AfxExtractSubString(cstrDHCP, cstrBackupData, 3, '|');
  //AfxExtractSubString(cstrIP, cstrBackupData, 4, '|');
  //AfxExtractSubString(cstrSubnet, cstrBackupData, 5, '|');
  //AfxExtractSubString(cstrGateway, cstrBackupData, 6, '|');
  //AfxExtractSubString(cstrDNS, cstrBackupData, 7, '|');

  if(cstrDHCP == _T("1"))
  {
    int iReturn = AfxMessageBox(_T("This backup is from a DHCP enabled configuration. Applying this as a static IP Address might not be helpful. Do you want to continue ?"), MB_YESNO);
    if(iReturn == IDNO)
      return;
  }

  //{{Reg Hive Restore
	SetPrivilege(SE_BACKUP_NAME, TRUE);
	SetPrivilege(SE_RESTORE_NAME, TRUE);

  CString cstrKey = _T("SYSTEM\\CurrentControlSet\\services\\Tcpip\\Parameters\\Interfaces\\");
  cstrKey += cstrGUID;
  HKEY hKey = NULL;
  LSTATUS iReturn = RegOpenKeyEx(HKEY_LOCAL_MACHINE, (LPCTSTR)cstrKey, 0, KEY_ALL_ACCESS, &hKey);
  if(iReturn == ERROR_SUCCESS)
  {
    HKEY hKeyToRestore = NULL;
    LSTATUS iStatus = RegDeleteKey(HKEY_LOCAL_MACHINE, (LPCTSTR)cstrKey);
    RegCloseKey(hKey);

    iStatus = RegCreateKeyEx(HKEY_LOCAL_MACHINE, (LPCTSTR)cstrKey, 0, 0, REG_OPTION_BACKUP_RESTORE, KEY_ALL_ACCESS, 0, &hKeyToRestore, 0);
		
    CString cstrHiveFile;
    AfxExtractSubString(cstrHiveFile, cstrFileName, 0, '.');
    cstrHiveFile += _T(".dat");

    iStatus = RegRestoreKey(hKeyToRestore, (LPCTSTR)cstrHiveFile, REG_FORCE_RESTORE);
    RegCloseKey(hKeyToRestore);
    
    Sleep(500);
    CString cstrSelectionAdapter = m_arrAdapterData[m_iCurrentConnectionIndex].GetAt(0);
    RestartAdapter(cstrSelectionAdapter);
    Sleep(500);
    RefreshAdapterInfo();
    AfxMessageBox(_T("IP configuration successfully restored"));
  }

  SetPrivilege(SE_BACKUP_NAME, FALSE);
  SetPrivilege(SE_RESTORE_NAME, FALSE);	
}

BOOL CIPMACManagerDlg::SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	HANDLE hToken; 

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
	if ( !LookupPrivilegeValue(NULL, lpszPrivilege, &luid) )    
		return FALSE; 
	
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
	    tp.Privileges[0].Attributes = 0;

	AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 
	return ( (GetLastError()!=ERROR_SUCCESS)?FALSE:TRUE);
}

void CIPMACManagerDlg::OnMinimizeButtonClick()
{
  ShowWindow(SW_MINIMIZE);
}

void CIPMACManagerDlg::OnCloseButtonClick()
{
  ::PostMessage(this->GetSafeHwnd(), WM_CLOSE, 0, 0);
}
