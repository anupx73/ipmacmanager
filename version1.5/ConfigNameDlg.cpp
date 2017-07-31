// ConfigNameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IPMACManager.h"
#include "ConfigNameDlg.h"
#include "IPMACManagerDlg.h"

extern CIPMACManagerDlg *g_pIPMACManagerDlg;

#define BTN_ID_GO 710
// CConfigNameDlg dialog

CRect rcBtnGO(223, 32, 285, 55);
CRect rcTextConfig(8, 6, 285, 20);
CRect rcTextLimit(8, 54, 285, 70);

CString g_cstrTextConfig = _T("Config name ?");
CString g_cstrTextLimit  = _T("30 characters max");

IMPLEMENT_DYNAMIC(CConfigNameDlg, CDialog)

CConfigNameDlg::CConfigNameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigNameDlg::IDD, pParent)
{
  m_pBtnGo = NULL;
}

CConfigNameDlg::~CConfigNameDlg()
{
}

void CConfigNameDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT_CONFIG_NAME, mEdit);
}


BEGIN_MESSAGE_MAP(CConfigNameDlg, CDialog)
	ON_WM_PAINT()
  ON_WM_LBUTTONDOWN()
  ON_BN_CLICKED(BTN_ID_GO, OnGoButtonClick)
END_MESSAGE_MAP()


// CConfigNameDlg message handlers

void CConfigNameDlg::LoadImagesFromResource()
{
  BITMAP bm = {0};
  //Back img
  m_BmpImg[0].LoadBitmap(IDB_BITMAP5);
  m_BmpImg[0].GetBitmap(&bm);
  m_SizBmpImg[0] = CSize(bm.bmWidth, bm.bmHeight);
  //SetWindowPos(NULL, 0, 0, bm.bmWidth, bm.bmHeight, SWP_NOMOVE);

  AdjustWindowPosition(&bm);
}

void CConfigNameDlg::OnPaint()
{
	CPaintDC dc(this);
  CDC pdc;
  pdc.CreateCompatibleDC(&dc);
  CBitmap *oldBmp = pdc.SelectObject(&m_BmpImg[0]);
  dc.BitBlt(0, 0, m_SizBmpImg[0].cx, m_SizBmpImg[0].cy, &pdc, 0, 0, SRCCOPY);
  dc.SelectObject(&oldBmp);

  dc.SetBkMode(TRANSPARENT);
  dc.SelectObject(&g_pIPMACManagerDlg->m_txtHeadingFont);
  dc.SetTextColor(RGB(255, 255, 255));
  dc.DrawText(g_cstrTextConfig, g_cstrTextConfig.GetLength(), rcTextConfig, DT_LEFT);

  dc.SelectObject(&g_pIPMACManagerDlg->m_txtFont);
  dc.SetTextColor(RGB(0, 0, 0));
  dc.DrawText(g_cstrTextLimit, g_cstrTextLimit.GetLength(), rcTextLimit, DT_LEFT);
}

BOOL CConfigNameDlg::OnInitDialog()
{
  CDialog::OnInitDialog();

  LoadImagesFromResource();
  CreateButton();
  mEdit.SetLimitText(30);
  return TRUE;
}


BOOL CConfigNameDlg::PreTranslateMessage(MSG *pMsg)
{
  if((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
  {
    switch(pMsg->wParam)
    {
    case VK_ESCAPE:
      CloseConfigNameDialog();
      return TRUE;

    case VK_RETURN:
      if(pMsg->message == WM_KEYDOWN)
        OnGoButtonClick();
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

void CConfigNameDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
  //CString cstr;
  //cstr.Format(_T("X: %d, Y: %d"), point.x, point.y);
  //AfxMessageBox(cstr);

  if((point.x >= 0 && point.x <= 290) && (point.y >= 0 && point.y <= 12))
  {
    PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
  }

  CDialog::OnLButtonDown(nFlags, point);
}

LRESULT CConfigNameDlg::OnNcHitTest(UINT uParam, LONG lParam)
{
  int xPos = LOWORD(lParam);
  int yPos = HIWORD(lParam);
  UINT nHitTest = CDialog::OnNcHitTest(CSize(xPos, yPos));

  return (nHitTest == HTCLIENT) ? HTCAPTION : nHitTest;
}

void CConfigNameDlg::CreateButton()
{
  m_pBtnGo = new CButtonST;
  if(m_pBtnGo != NULL)
  {
    m_pBtnGo->Create(_T("GO"), WS_CHILD | WS_GROUP | WS_TABSTOP, rcBtnGO, this, BTN_ID_GO);
    m_pBtnGo->SetFont(&g_pIPMACManagerDlg->m_txtFont);
    m_pBtnGo->DrawBorder(FALSE);

    m_pBtnGo->SetColor(CButtonST::BTNST_COLOR_BK_IN, RGB(197, 59, 5));
    m_pBtnGo->SetColor(CButtonST::BTNST_COLOR_BK_OUT, RGB(223, 64, 2));
    m_pBtnGo->SetColor(CButtonST::BTNST_COLOR_BK_FOCUS, RGB(223, 64, 2));

    m_pBtnGo->SetColor(CButtonST::BTNST_COLOR_FG_IN, RGB(255, 255, 255));
    m_pBtnGo->SetColor(CButtonST::BTNST_COLOR_FG_OUT, RGB(255, 255, 255));
    m_pBtnGo->SetColor(CButtonST::BTNST_COLOR_FG_FOCUS, RGB(255, 255, 255));
  }
}

void CConfigNameDlg::OnGoButtonClick()
{
  CString cstrConfgiName;
  mEdit.GetWindowText(cstrConfgiName);
  if(cstrConfgiName.IsEmpty() == true)
  {
    AfxMessageBox(_T("Give your backup a name to identify later"));
    return;
  }

  g_pIPMACManagerDlg->CreateIPBackup(cstrConfgiName);
  g_pIPMACManagerDlg->EnableWindow();
  ShowWindow(FALSE);
}

void CConfigNameDlg::Initialize()
{
  g_pIPMACManagerDlg->EnableWindow(FALSE);
  mEdit.SetWindowText(_T(""));
  m_pBtnGo->ShowWindow(TRUE);
  ShowWindow(TRUE);
}

void CConfigNameDlg::AdjustWindowPosition(BITMAP *bm)
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

  SetWindowPos(&wndTop, rcOwner.left + (rc.right / 2) + 67, rcOwner.top + (rc.bottom / 2) + 60, bm->bmWidth, bm->bmHeight, SWP_SHOWWINDOW); 
}

void CConfigNameDlg::CloseConfigNameDialog()
{
  g_pIPMACManagerDlg->EnableWindow();
  ShowWindow(FALSE);
}
