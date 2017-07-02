/*

IP-MAC Manager Version 1.0.
Copyright © Anupam Saha, 2009.
mail: hyphen.on@gmail.com

*/
#define _CRT_SECURE_NO_WARNINGS
#include "splash.h"
#include "Headers.h"
#include "resource.h"
#include "Global.h"
#include "AdapterRestart.h"
#include "SetPrivilege.h"
#include "UnicodeCast.h"
#include "DeleteAllFile.h"

bool IsValidMAC(wchar_t * str) {
  if (wcslen(str) != 12) 
	  return false;
  for (int i = 0; i < 12; i++) 
  {
    if ((str[i] < '0' || str[i] > '9') && (str[i] < 'a' || str[i] > 'f') && (str[i] < 'A' || str[i] > 'F')) 
      return false;
   }
  return true;
  }


void No_NIC (HWND)
{
	HWND ST5 = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", L"**No NIC Found. For more info see FAQ in Help.", WS_CHILD, 26, 286, 250, 20, hMainClone, 0, hIns, 0);
	SendMessage(ST5, WM_SETFONT, (WPARAM)hFont, 0);
	ShowWindow(ST5, SW_SHOWNORMAL);
}
LRESULT CALLBACK CurIPDlgProc(HWND hCurIP, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
	{
		//SendMessage(hCurIP, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIcon);
		SetWindowText(hCurIP, BufferS_VI_copy);
	}break;

	case WM_PAINT:
	{
		HDC hdc;
		PAINTSTRUCT ps;
		RECT rect, rc, rcc, rc2;
		rect.bottom = 160; 			rect.left = 140; 		rect.right = 250; 		rect.top = 82;
		rc.left = rect.left;		rc.right=rect.right;					rc.bottom=rect.bottom;				rc.top=rect.top+14; 
		rcc.left=rect.left;		rcc.right=rect.right;					rcc.bottom=rect.bottom;				rcc.top=rc.top+14;
		rc2.left=rect.left;		rc2.right=rect.right;				rc2.bottom=rect.bottom+40;		rc2.top=rcc.top+14;
		
		hdc= BeginPaint(hCurIP, &ps);
		SetBkMode(hdc,TRANSPARENT);
		SelectObject(hdc,hFont);
		DrawText(hdc, IP, -1, &rect, 0);
		DrawText(hdc, SubnetMask, -1, &rc, 0);
		DrawText(hdc, DefaultGateway, -1, &rcc, 0);
		DrawText(hdc, DNS, -1, &rc2, 0);
		EndPaint(hCurIP, &ps);
	}break;

	case WM_CLOSE:	
		EndDialog(hCurIP, 0);
		break;

	case WM_COMMAND:
	{
		switch(wParam)
		{
			case CURRENTIP_OK:
				EndDialog(hCurIP, 0);
				break;
		}
	}break;
	
	}
	return 0;
}


LRESULT CALLBACK AboutDlgProc(HWND hAbout, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_CLOSE:
			EndDialog(hAbout, 0);
			break;

		case WM_COMMAND:
		{
			switch(wParam)
			{
			case About_OK:
				EndDialog(hAbout, 0);
				break;
			}
		}break;
	}
	return 0;
}


LRESULT CALLBACK RestoreDlg(HWND hReIP, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_INITDIALOG:
		{
			//SendMessage(hReIP, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIcon);
			SetWindowText(hReIP, BufferS_RI_copy);
			REIP_BAK_RADIO (hReIP);
			CheckRadioButton(hReIP, ID_RADIO0, ID_RADIO7, ID_RADIO0);
		}break;
		
		case WM_PAINT:
		{
			DWORD Buffer;
			wchar_t Connec_Re[100], WinDir_Chk[MAX_PATH];
			wchar_t File_Str[200] = L"";
			RECT rC;
			HDC hdC;
			PAINTSTRUCT pS;
			rC.bottom = 300, rC.left = 140, rC.right = 300, rC.top = 91;

			GetWindowsDirectory(WinDir_Chk, MAX_PATH);
			wcscat_s(WinDir_Chk, MAX_PATH, L"\\IPMAC\\");
			SendMessage(hReIP, WM_GETTEXT, (WPARAM)100, (LPARAM)Connec_Re);
			wcscat_s(WinDir_Chk, MAX_PATH, Connec_Re);
			wcscat_s(WinDir_Chk, MAX_PATH, L"\\LOG.LOG");
			HANDLE hRead = CreateFile(WinDir_Chk, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
			ReadFile(hRead, File_Str, 200, &Buffer, 0);
			CloseHandle(hRead);

			hdC= BeginPaint(hReIP, &pS);
			SetBkMode(hdC,TRANSPARENT);
			SelectObject(hdC,hFont);
			DrawText(hdC, File_Str, -1, &rC, 0);
			EndPaint(hReIP, &pS);
		}break;

		case WM_CLOSE:
			EndDialog(hReIP, 0);
			break;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case RE_SELECTED:
				REIP_RE_SELECTED(hReIP);
				DestroyWindow(ST);
				MAIN_IP_STATIC_SHOW(hMainClone);
				break;

			case RE_CLOSE:
				EndDialog(hReIP,0);
				break;

			case RE_DE:
				RE_DEL_ALL (hReIP);
				EndDialog (hReIP, 0);
				DestroyWindow(ST1);
				MAIN_IP_BAK_COUNT (hMainClone);
				break;	

			case RE_DE_SE:
				RE_DEL_SELECTED (hReIP);
				EndDialog (hReIP, 0);
				DestroyWindow(ST1);
				MAIN_IP_BAK_COUNT (hMainClone);
				break;	
			}
		}break;

	}//switch end
	return 0;
}


LRESULT CALLBACK SpoofMACDlg(HWND hspMAC, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
		case WM_INITDIALOG:
		{
			//SetWindowText(hspMAC, BufferS_SP_copy);
			//SendMessage(hspMAC, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
			EDITBALLOONTIP tip;
			tip.cbStruct = sizeof(EDITBALLOONTIP);
			tip.pszText = L"Enter 2-Digit Hex No.";
			tip.pszTitle = L"";
			tip.ttiIcon = TTI_INFO;

			for(int i=0; i<8; i++) {
				SendDlgItemMessage(hspMAC, ID_EDIT1 + i, EM_LIMITTEXT, (WPARAM)2, 0);
			}
			SendDlgItemMessage(hspMAC, ID_EDIT1, EM_SHOWBALLOONTIP, 0, (LPARAM)&tip);
		}break;

		case WM_PAINT:
		{
			wchar_t Adap [300] = L"Adapter       : ";
			wchar_t ConncSP [300] = L"Connection : ";
			wcscat_s(Adap, 300, BufferS_SP_copy);
			wcscat_s(ConncSP, 300, Connec_spMAC);
			RECT rC, rC2;
			HDC hdC;
			PAINTSTRUCT pS;
			rC.bottom = 100, rC.left = 11, rC.right = 400, rC.top = 76;
			rC2.bottom = 200, rC2.left = 11, rC2.right = 400, rC2.top = 94;

			hdC= BeginPaint(hspMAC, &pS);
			SetBkMode(hdC,TRANSPARENT);
			SelectObject(hdC,hFont);
			DrawText(hdC, Adap, -1, &rC, 0);
			DrawText(hdC, ConncSP, -1, &rC2, 0);
			EndPaint(hspMAC, &pS);
		}break;

		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case SP_MAC_CLOSEBTN:
					EndDialog(hspMAC,0);
					break;

				case SP_MAC_SETMACBTN:
					SP_SETMAC_BTN_MAC_CHK (hspMAC);
					DestroyWindow(ST3);
					DestroyWindow(ST4);
					MAIN_MAC_STATIC_SHOW (hMainClone);
					MAC_SPOOF_CHK (hMainClone);
					break;

				case SP_MAC_RANDBTN:
					SP_RAND_MAC (hspMAC);
					break;
			}
				
			int nControlID = LOWORD (wParam);
			switch(nControlID)
			{
				case ID_EDIT1:
				case ID_EDIT2:
				case ID_EDIT3:
				case ID_EDIT4:
				case ID_EDIT5:
				case ID_EDIT6:
				{
					switch(HIWORD(wParam))
					{
						case EN_KILLFOCUS:
							wchar_t EditText[20] = L"";
							GetDlgItemText(hspMAC, nControlID, EditText, sizeof(EditText));
							_wcsupr_s(EditText, sizeof(EditText));
							SetDlgItemText(hspMAC, nControlID, EditText);
							break;
					}
				}break;
			}

		}break;																	//WM_COMMAND end

		case WM_CLOSE:
			EndDialog(hspMAC,0);
			break;
	}
	return 0;
}
LRESULT CALLBACK MainDlgProc(HWND hMain, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
	{
		SetClassLong(hMain, GCL_HICON, (LONG)LoadIcon (hIns, MAKEINTRESOURCE(IDI_ICON1)) );
		
		/*** new code ****/
		//SetWindowLong(hMain, GWL_EXSTYLE, GetWindowLong(hMain, GWL_EXSTYLE) | WS_EX_LAYERED);
		//SetLayeredWindowAttributes(hMain, 0, (255 * 70) / 100, LWA_ALPHA);
		/*** end ***/

		hMainClone = hMain;
		MAIN_ComboMAC(hMain);
		MAIN_ComboIP(hMain);
		MAIN_IP_STATIC_SHOW (hMain);
		MAIN_IP_BAK_COUNT (hMain);
		MAIN_ADAPTER_RETRV (hMain);
		RegDeleteKey(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}\\Descriptions");
			
		if (SendMessage(GetDlgItem(hMain, COMBO_MAC), CB_SETCURSEL, 0, 0) == CB_ERR)
			No_NIC (hMain);

		else if (SendMessage(GetDlgItem(hMain, COMBO_MAC), CB_SETCURSEL, 0, 0) != CB_ERR)
		{	
			MAIN_MAC_STATIC_SHOW (hMain);
			MAC_SPOOF_CHK (hMain);
		}

	}break;

	case WM_CLOSE:
		PostQuitMessage(0);
		break;

	case WM_CTLCOLORSTATIC:
		SetBkMode( ( HDC )wParam, TRANSPARENT );
		SetTextColor( (HDC)wParam, RGB(255, 255, 255));
		return ( LRESULT )GetStockObject( NULL_BRUSH );
		break;

/*	case WM_CTLCOLORBTN:
		SetBkMode( ( HDC )wParam, TRANSPARENT );
		//SetTextColor( (HDC)wParam, RGB(255, 255, 255));
		return ( LRESULT )GetStockObject( NULL_BRUSH );
		break;	*/

	case WM_COMMAND:
		{
		switch(LOWORD(wParam))
		{
			case COMBO_IP:
			{
				switch(HIWORD(wParam))
				{
					case CBN_SELCHANGE:
						DestroyWindow(ST);
						DestroyWindow(ST1);
						DestroyWindow(ST2);

						MAIN_IP_STATIC_SHOW (hMain);
						MAIN_IP_BAK_COUNT (hMain);
						MAIN_ADAPTER_RETRV (hMain);
						break;															
				}
			}break;																								// case COMBO_IP end
			
			case COMBO_MAC:
			{
				switch(HIWORD(wParam))
				{
					case CBN_SELCHANGE:
						DestroyWindow(ST3);
						DestroyWindow(ST4);
						MAIN_MAC_STATIC_SHOW (hMain);
						MAC_SPOOF_CHK (hMain);
						break;
				}
			}break;

			case B_EXIT:
				PostQuitMessage(0);
				break;
		
			case B_ABOUT:
				DialogBox(hIns, MAKEINTRESOURCE(IDD_ABOUT), hMain, (DLGPROC)AboutDlgProc);
				break;

			case B_VIEWIP:
				MAIN_BUTTON_VIEWIP (hMain);
				break;												
		
			case B_BACKUPIP:
				MAIN_BUTTON_BACKUPIP (hMain);	
				break;																	

			case B_RESTOREIP:
				MAIN_BUTTON_RESTOREIP (hMain);
				break;				

			case B_CHANGEMAC:
				MAIN_BUTTON_SPOOFMAC (hMain);
				break;

			case B_RESTOREMAC:
				MAIN_RESTORE_MAC_YES_NO (hMain);
				DestroyWindow(ST3);
				DestroyWindow(ST4);
				MAIN_MAC_STATIC_SHOW (hMain);
				MAC_SPOOF_CHK (hMain);
				break;

			case B_HELP:
				HINSTANCE HelpEx = ShellExecute(hMain, L"open", L"Help.txt", 0, 0, SW_SHOWNORMAL);
				if ((int)HelpEx  == ERROR_FILE_NOT_FOUND)
					MessageBox(hMain, L"Error locating help file. Please reinstall the application.", L"Error", MB_ICONERROR | MB_OK);
				break;

	}break;																										//WM_COMMAND break
	}																												//switch (Msg) End
  }
  return 0;
}


void MAIN_ComboIP (HWND hMain)
{
			HWND comboIP = GetDlgItem(hMain, COMBO_IP);
			HKEY hListKey, hCo;
			wchar_t key[512], ConnectionName[512][6];
			DWORD BufSize= 512;

			int l=0;
			RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}", 0, KEY_READ, &hListKey);
			while ( RegEnumKeyEx(hListKey, l++, key, &BufSize, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS)
			{		
				
				wcscat_s(key, 512, L"\\Connection");
										
				RegOpenKeyEx(hListKey, key, 0, KEY_READ, &hCo);
				BufSize = 512;
				int count2 =0;
				crap = 0;
				if (RegQueryValueEx(hCo, L"Name", 0, &crap, (LPBYTE)ConnectionName[count2], &BufSize) == ERROR_SUCCESS) 
				{
					for(int k=0; k<=count2; k++)
						SendMessage(comboIP, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(ConnectionName[count2]));
				}													
				RegCloseKey(hCo);
				BufSize=512;
			}		
	 
			RegCloseKey(hListKey);
			SendMessage(comboIP, CB_SETCURSEL, 0, 0);
}

void MAIN_IP_STATIC_SHOW (HWND hMain)
{
		WCHAR BufferS1[300];										// comboIP::  IP show in static
		HKEY hListKey1, hCo1;
		wchar_t key1[512], ConnectionName1[512], copy1[512];
		DWORD BufSize1= 512;
		int l1=0;
		comboIP = GetDlgItem(hMain, COMBO_IP);
		SendMessage(comboIP, CB_GETLBTEXT, (int)SendMessage(comboIP, CB_GETCURSEL, 0, 0), (LPARAM)BufferS1);

		RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}", 0, KEY_READ, &hListKey1);
		while (RegEnumKeyEx(hListKey1, l1++, key1, &BufSize1, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS)
		{		
			wcscpy_s(copy1, 512, key1);	
			wcscat_s(key1, 512, L"\\Connection");
	
			RegOpenKeyEx(hListKey1, key1, 0, KEY_READ, &hCo1);
			BufSize1 = 512;
			crap = 0;
			RegQueryValueEx(hCo1, L"Name", 0, &crap, (LPBYTE)ConnectionName1, &BufSize1);
	
			if (wcscmp(ConnectionName1, BufferS1) == 0)
			{
				HKEY fqury1;
				DWORD cbIP1 =80;
				WCHAR Interfaces1[200]  = L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
				wcscat_s (Interfaces1, 200, copy1);
			
				RegOpenKeyEx(HKEY_LOCAL_MACHINE, Interfaces1, 0, KEY_READ, &fqury1);
				RegQueryValueEx (fqury1, L"IPAddress", 0, &lpTypeR, (LPBYTE)&IP, &cbIP1);
			}
			RegCloseKey(hCo1);
			BufSize1=512;
		} // end of while loop
		RegCloseKey(hListKey1);											//ip chk end
		WCHAR IPcon1[100] = L"IP Address\t: ";
		wcscat_s (IPcon1, 100, IP);
		//SetDlgItemText(hMain, STATIC_1, (LPCWSTR)IPcon1);				// // comboIP IP show in static end
		ST = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", IPcon1, WS_CHILD, 26, 162, 180, 20, hMain, 0, hIns, 0);
		SendMessage(ST, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), 0);
		ShowWindow(ST, SW_SHOWNORMAL);
}


void MAIN_IP_BAK_COUNT (HWND hMain)
{
		wchar_t Wi[300];
		WCHAR BufferS1[300];
		SendMessage(comboIP, CB_GETLBTEXT, (int)SendMessage(comboIP, CB_GETCURSEL, 0, 0), (LPARAM)BufferS1);
		GetWindowsDirectory(Wi, MAX_PATH);									//backup count
		wcscat_s(Wi, 300, L"\\IPMAC\\");
		wcscat_s(Wi, 300, BufferS1);
		wcscat_s(Wi, 300, L"\\*");
		//MessageBox(0,WinDir,0,0);
		
		WIN32_FIND_DATA FindFileData1;
		wchar_t coun_s[40];
		HANDLE hFind1 = FindFirstFile(Wi, &FindFileData1);
		int count_=0;
		while(FindNextFile(hFind1, &FindFileData1) != 0)
			count_+=1;

		FindClose(hFind1);
		if(count_!=0)
			count_-= 2;
		
		swprintf_s(coun_s, 40, L"%d", count_);
		//MessageBox(0,coun_s,0,0);
		wchar_t backFound1[20] = L"Backups\t: ";
		wcscat_s(backFound1, 20, coun_s);

		ST1 = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", 0, WS_CHILD, 26, 179, 180, 20, hMain, 0, hIns, 0);
		ShowWindow(ST1, SW_SHOWNORMAL);
		SendMessage(ST1, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(backFound1));
		SendMessage(ST1, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), 0);
}

void MAIN_ADAPTER_RETRV (HWND hMain)
{
	HKEY hNew, hCoN, hNew2, hCoN2;
	WCHAR BufferS1[300];
	int n1 = 0;
	int n2 = 0;
	wchar_t keyN[512],		ConnectionNameN[300],		keyN2[512],	copyN[512],	IDN[512],	Adapter[512];
	wchar_t AdapterN[600] = L"Adapter Name\t: ";
	DWORD BufN= 512;
	DWORD BufN2= 512;
	SendMessage(comboIP, CB_GETLBTEXT, (int)SendMessage(comboIP, CB_GETCURSEL, 0, 0), (LPARAM)BufferS1);

	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}", 0, KEY_READ, &hNew);
	while (RegEnumKeyEx(hNew, n1++, keyN, &BufN, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS)
	{		
		wcscpy_s(copyN, 512, keyN);	
		wcscat_s(keyN, 512, L"\\Connection");

		RegOpenKeyEx(hNew, keyN, 0, KEY_READ, &hCoN);
		BufN = 512;
		crap = 0;
		RegQueryValueEx(hCoN, L"Name", 0, &crap, (LPBYTE)ConnectionNameN, &BufN);

		if (wcscmp(ConnectionNameN, BufferS1) == 0)
		{
			//MessageBox(0,(LPCWSTR)ConnectionNameN,0,0);
			RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}", 0, KEY_READ, &hNew2);
			while (RegEnumKeyEx(hNew2, n2++, keyN2, &BufN2, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS)
			{
				RegOpenKeyEx(hNew2, keyN2, 0, KEY_READ, &hCoN2);
				BufN2 = 512;
				crap = 0;
				RegQueryValueEx(hCoN2, L"NetCfgInstanceId", 0, &crap, (LPBYTE)IDN, &BufN2);
				if(wcscmp(IDN, copyN) == 0)
				{
					BufN2 = 512;
					RegQueryValueEx(hCoN2, L"DriverDesc", 0, &crap, (LPBYTE)Adapter, &BufN2);
					wcscat_s (AdapterN, 600, Adapter);
					//MessageBox(0,AdapterN,0,0);
					ST2 = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", 0, WS_CHILD, 26, 144, 380, 20, hMain, 0, hIns, 0);
					ShowWindow(ST2, SW_SHOWNORMAL);
					SendMessage(ST2, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(AdapterN));
					SendMessage(ST2, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), 0);
				}
			}
		}
		//RegCloseKey(hCoN2);
		BufN=512;
	}
	RegCloseKey(hNew);
}
void MAIN_BUTTON_VIEWIP (HWND hMain)
{
		WCHAR BufferS[300];
		HKEY hListKey, hCo;
		wchar_t key[512], ConnectionName[512], copy[512];
		DWORD BufSize= 512;
		int l=0;
		int index = SendMessage(comboIP, CB_GETCURSEL, 0, 0);
		SendMessage(comboIP, CB_GETLBTEXT, index, (LPARAM)BufferS);
		wcscpy_s(BufferS_VI_copy, 300, BufferS);
		//MessageBox(0, BufferS_VI_copy, 0,0);

		RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}", 0, KEY_READ, &hListKey);
		while (RegEnumKeyEx(hListKey, l++, key, &BufSize, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS)
		{		
			wcscpy_s(copy, 512, key);	
			wcscat_s(key, 512, L"\\Connection");
			
			RegOpenKeyEx(hListKey, key, 0, KEY_READ, &hCo);
			BufSize = 512;
			crap = 0;
			RegQueryValueEx(hCo, L"Name", 0, &crap, (LPBYTE)ConnectionName, &BufSize);
			
			if (wcscmp(ConnectionName, BufferS) == 0)
			{
					HKEY fqury;
					DWORD cbIP =80;
					DWORD cbDef =80;
					DWORD cbSubnet = 80;
					DWORD cbDNS = 200;
					WCHAR Interfaces[200]  = L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
					lstrcat (Interfaces, copy);
					
					RegOpenKeyEx(HKEY_LOCAL_MACHINE, Interfaces, 0, KEY_READ, &fqury);
					RegQueryValueEx (fqury, L"IPAddress", 0, &lpTypeR, (LPBYTE)&IP, &cbIP);
					RegQueryValueEx (fqury, L"DefaultGateway", 0, &lpTypeR, (LPBYTE)&DefaultGateway, &cbDef);
					RegQueryValueEx (fqury, L"SubnetMask", 0, &lpTypeR, (LPBYTE)&SubnetMask, &cbSubnet);
					RegQueryValueEx (fqury, L"SubnetMask", 0, &lpTypeR, (LPBYTE)&SubnetMask, &cbSubnet);
					RegQueryValueEx (fqury, L"NameServer", 0, &lpTypeR, (LPBYTE)&DNS, &cbDNS);

					for(int i=0; i<=200; i++)
					{
						switch (DNS[i])
						{			
							case ',' :
								DNS[i]='\n';
								break;	
						}
					}

			}  //end if
			
			RegCloseKey(hCo);
			BufSize=512;
		}			
		RegCloseKey(hListKey);
		DialogBox(hIns, MAKEINTRESOURCE(IDD_CURRENTIP), hMain, (DLGPROC)CurIPDlgProc);
}

LRESULT CALLBACK DLGDesc(HWND hDesc, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch(Msg)	{
		case WM_INITDIALOG:	{
				WCHAR IP_Desc[200] = L"Description for IP: ";
				wcscat_s(IP_Desc, 200, IP);
				SetDlgItemText(hDesc, IDC_Desc, IP_Desc);
				SendDlgItemMessage(hDesc, IDC_EDIT2, EM_LIMITTEXT, (WPARAM)14, 0);
			}
			break;

		case WM_COMMAND:
			switch(LOWORD (wParam))	{
				case IDOK:
				{
					GetDlgItemText(hDesc, IDC_EDIT2, Desc, 200);	
					if (wcscmp(Desc, L"") != 0)	{
						EndDialog(hDesc, 0);
						MAIN_BUTTON_BACKUPIP_FUNCTION(hMainClone);
					}
					else
						MessageBox(hDesc, L"Please enter description for identifying IP configuration.", L"No description", MB_ICONEXCLAMATION | MB_OK);
				}break;
			}
			break;
		
		case WM_CLOSE:
			//GetDlgItemText(hDesc, IDC_EDIT2, Desc, 200);
			EndDialog(hDesc, 0);
			break;
	}
	return 0;

}
void MAIN_BUTTON_BACKUPIP (HWND hMain)
{
		DialogBox(hIns, MAKEINTRESOURCE(IDD_Enter_Desc), hMain, (DLGPROC)DLGDesc);
}

void MAIN_BUTTON_BACKUPIP_FUNCTION (HWND hMain)
{
		wchar_t BufferS_[300], key_[512], ConnectionName_[512], copy_[512];
		HKEY hListKey_, hCo_, fqury_;
		DWORD BufSize_= 512;
		int l_=0;
				
		SetPrivilege(SE_BACKUP_NAME, TRUE);
		int index_ = SendMessage(comboIP, CB_GETCURSEL, 0, 0);
		SendMessage(comboIP, CB_GETLBTEXT, index_, (LPARAM)BufferS_);
		
		RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}", 0, KEY_READ, &hListKey_);
		
		while (RegEnumKeyEx(hListKey_, l_++, key_, &BufSize_, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS)
		{		
			wcscpy_s(copy_, 512, key_);	
			wcscat_s(key_, 512, L"\\Connection");
			
			RegOpenKeyEx(hListKey_, key_, 0, KEY_READ, &hCo_);
			BufSize_ = 512;
			RegQueryValueEx(hCo_, L"Name", 0, &crap, (LPBYTE)ConnectionName_, &BufSize_);
			
			if (wcscmp(ConnectionName_, BufferS_) == 0)
			{		
					DWORD cbIP_ = 80;
					WCHAR Interfaces_[300]  = L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
					wcscat_s (Interfaces_, 300, copy_);
					RegOpenKeyEx(HKEY_LOCAL_MACHINE, Interfaces_, 0, KEY_READ, &fqury_);
					RegQueryValueEx (fqury_, L"IPAddress", 0, &lpTypeR, (LPBYTE)&IP, &cbIP_);
			}
			RegCloseKey(hCo_);
			BufSize_=512;
			
		}
		WCHAR WinDir_IP[300], WinDir_IPMAC[300];
		GetWindowsDirectory(WinDir, MAX_PATH);							//directory create
		wcscat_s(WinDir, 300, L"\\IPMAC\\");
		if (_waccess(WinDir, 0) != 0)
			CreateDirectory(WinDir, 0);		
																							
		wcscpy_s(WinDir_IP, 300, WinDir);
		//MessageBox(0, (LPCWSTR)Desc,0,0);
		wcscat_s(WinDir, 300, IP);
		RegSaveKey(fqury_, WinDir, 0);									//WinDir == IP File Name
		RegCloseKey(hListKey_);	
		
		
		wchar_t Retrv[100];
		SendMessage(comboIP, CB_GETLBTEXT, (int)SendMessage(comboIP, CB_GETCURSEL, 0, 0), (LPARAM)Retrv);

		wcscat_s(WinDir_IP, 300, Retrv);		//WinDir_IP == IP Folder Name
		if (_waccess(WinDir_IP, 0) != 0)
			CreateDirectory(WinDir_IP, 0);		
		
		wcscat_s(WinDir_IP, 300, L"\\");
		wcscpy_s(WinDir_IPMAC, 300, WinDir_IP);			//copied
		wcscat_s(WinDir_IP, 300, IP);
		wcscat_s(WinDir_IP, 300, L".DAT");
		MoveFileEx(WinDir, WinDir_IP, MOVEFILE_REPLACE_EXISTING);			//directory end
		
		HWND Bk_w = CreateDialog(hIns, MAKEINTRESOURCE(SLEEP_BAKUP), hMain, 0);
		ShowWindow(Bk_w, SW_SHOWNORMAL);
		UpdateWindow(Bk_w);
		Sleep(2000);
		DestroyWindow(Bk_w);
		
		wcscat_s(WinDir_IPMAC, 300, L"log.txt");				//log file
		//MessageBox(0,WinDir_IPMAC, 0,0);
		WCHAR CurDate[16], CurTime[16];
		//_wstrdate_s(CurDate, 16);
		
		time_t now = time(0);
		struct tm *tm = localtime(&now);
		
		swprintf_s (CurDate, 16, L"%d/%d/%d", tm->tm_mday, tm->tm_mon+1, tm->tm_year+1900);
		//MessageBox(hMain,CurDate, 0,0);
		_wstrtime_s(CurTime, 16);

		wofstream of(WinDir_IPMAC, ios::out | ios::app);
		of<<IP<<L"             "<<Desc<<L"              "<<CurDate<<L" -- "<<CurTime<<endl;
		of.close();																						//log end
		
		wcscat_s(IP, 300, L" IP configuration backup created successfully.");	
		MessageBox(0, IP, L"Configuration Backup Complete", MB_OK | MB_ICONINFORMATION);
		DestroyWindow(ST1);
		MAIN_IP_BAK_COUNT (hMain);
		SetPrivilege(SE_BACKUP_NAME, FALSE);
}

void MAIN_BUTTON_RESTOREIP (HWND hMain)
{
	wchar_t BufferS_RI[300];
	SendMessage(comboIP, CB_GETLBTEXT, (int)SendMessage(comboIP, CB_GETCURSEL, 0, 0), (LPARAM)BufferS_RI);
	wcscpy_s(BufferS_RI_copy, 300, BufferS_RI);

	DialogBox(hIns, MAKEINTRESOURCE(IDD_RESTOREDAILOG), hMain, (DLGPROC)RestoreDlg);
}
void REIP_BAK_RADIO (HWND hReIP)
{
	WCHAR WinDirChk[512], Chk_Exist[512], Msg_Conn_N[100];
	int i = 0;
	int height = 88;
	SendMessage(hReIP, WM_GETTEXT, (WPARAM)100, (LPARAM)Msg_Conn_N);
	GetWindowsDirectory(WinDirChk, MAX_PATH);
	wcscat_s(WinDirChk, 512, L"\\IPMAC\\");
	wcscat_s(WinDirChk, 512, BufferS_RI_copy);
	wcscpy_s(Chk_Exist, 512, WinDirChk);
	wcscat_s(WinDirChk, 512, L"\\*");
	
	if(_waccess(Chk_Exist, 0)!=0)	{	// Chk_Exist == 1394 connection	
		MessageBox(hReIP, L"No backup has been created yet for this connection.\n\n         To create backup press Backup Config.", Msg_Conn_N, MB_OK | MB_ICONEXCLAMATION);
		EndDialog(hReIP,0);
	}
		
	wcscat_s(Chk_Exist, 512, L"\\log.txt");
	char ReadLine[1024][10];

	ifstream inFile(Chk_Exist);
	while	(inFile.getline(ReadLine[i], sizeof(ReadLine), '\n'))		{
		hRadio[i] = CreateWindowEx(0, L"BUTTON", 0, WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON, 21, height, 400, 20, hReIP, (HMENU)1034+i, hIns, 0);
		SetWindowText(hRadio[i], Unicode_Cast(ReadLine[i]));
		SendMessage(hRadio[i], WM_SETFONT, (WPARAM)hFont, 0);		
		height+=26;
		i++;
	}
	inFile.close();
}

void REIP_RE_SELECTED (HWND hReIP)
{
	wchar_t IP_Store_bak[200], Connec_Retrv[100], WinDir[MAX_PATH];
	wchar_t ReadRadio[1024];
	wchar_t Restored_Msg [80] = L" IP restored successfully";
	wchar_t InterfaceS[300] = L"SYSTEM\\CurrentControlSet\\Services\\Tcpip\\Parameters\\Interfaces\\";
	int reserve;						
	HKEY hListKey1, hCo1, hSaved;
	wchar_t key1[512], ConnectionName1[512], copy1[512];
	DWORD BufSize1= 512;
	int l1=0;
	SetPrivilege(SE_BACKUP_NAME, TRUE);
	SetPrivilege(SE_RESTORE_NAME, TRUE);
	for(int j=0; j<8; j++) 	{
		if (SendMessage( hRadio[j], BM_GETCHECK, 0, 0 ) == BST_CHECKED) 	{
			reserve = j;
			break;
		}
	}
	SendMessage(hRadio[reserve], WM_GETTEXT, (WPARAM)1024, (LPARAM)ReadRadio);
	wistringstream stream(ReadRadio);
	stream.getline(ReadRadio, sizeof(ReadRadio), ' ');
	//MessageBox(hReIP, ReadRadio, 0,0);
	
	wcscpy_s(IP_Store_bak, 200, ReadRadio);
	wcscat_s(IP_Store_bak, 200, Restored_Msg);
	wcscat_s(ReadRadio, 400, L".DAT");
	GetWindowText (hReIP, Connec_Retrv, 100);
	
	GetWindowsDirectory(WinDir, MAX_PATH);
	wcscat_s(WinDir, MAX_PATH, L"\\IPMAC\\");
	wcscat_s(WinDir, MAX_PATH, Connec_Retrv);
	wcscat_s(WinDir, MAX_PATH, L"\\");
	wcscat_s(WinDir, MAX_PATH, ReadRadio);				//WinDir == Saved file
	//MessageBox(hReIP, WinDir, 0,0);
	//MessageBox(0,WinDir,0,0);
	
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}", 0, KEY_READ, &hListKey1);
	while (RegEnumKeyEx(hListKey1, l1++, key1, &BufSize1, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS)
	{		
		wcscpy_s(copy1, 512, key1);	
		wcscat_s(key1, 512, L"\\Connection");

		RegOpenKeyEx(hListKey1, key1, 0, KEY_READ, &hCo1);
		BufSize1 = 512;
		crap = 0;
		RegQueryValueEx(hCo1, L"Name", 0, &crap, (LPBYTE)ConnectionName1, &BufSize1);
		if (wcscmp(ConnectionName1, Connec_Retrv) == 0)
		{
			wcscat_s(InterfaceS, 300, copy1);
			RegDeleteKey(HKEY_LOCAL_MACHINE, InterfaceS);
			RegCreateKeyEx(HKEY_LOCAL_MACHINE, InterfaceS, 0, 0, REG_OPTION_BACKUP_RESTORE, KEY_ALL_ACCESS, 0, &hSaved, 0);
			RegRestoreKey(hSaved, WinDir, 0);

			HWND Sleep_ReIP = CreateDialog(hIns, MAKEINTRESOURCE(SLEEP_REIP), hReIP, 0);
			ShowWindow(Sleep_ReIP, SW_SHOWNORMAL);
			UpdateWindow(Sleep_ReIP);
			Sleep(1600);
			DestroyWindow(Sleep_ReIP); 
						
			HWND Sleep_Adap = CreateDialog(hIns, MAKEINTRESOURCE(SLEEP_READAP), hReIP, 0);
			ShowWindow(Sleep_Adap, SW_SHOWNORMAL);
			UpdateWindow(Sleep_Adap);
			Adapter_Restart(Connec_Retrv);
			Sleep(600);
			DestroyWindow(Sleep_Adap); 

			MessageBox(hReIP, IP_Store_bak, L"IP Restored", MB_OK | MB_ICONINFORMATION);
		}
		RegCloseKey(hCo1);
		BufSize1=512;
	}
	RegCloseKey(hListKey1);
	SetPrivilege(SE_BACKUP_NAME, FALSE);
	SetPrivilege(SE_RESTORE_NAME, FALSE);	
}

void MAIN_ComboMAC (HWND hMain)
{
	HWND comboMAC = GetDlgItem(hMain, COMBO_MAC);
	PIP_ADAPTER_INFO pAdapterInfo, pAdapter;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
    {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
     }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
    {
        pAdapter = pAdapterInfo;
        while (pAdapter) 
		{
			LPWSTR Uni_cpy = Unicode_Cast(pAdapter->Description);
			//Uni_cpy [wcslen(Uni_cpy) - 27] = NULL;

			SendMessage(comboMAC, CB_ADDSTRING, 0, (LPARAM)Uni_cpy);
			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);
	SendMessage(comboMAC, CB_SETCURSEL, 0, 0);
}

void MAIN_MAC_STATIC_SHOW (HWND hMain)
{
	wchar_t mac_get_combo[200];
	comboMAC = GetDlgItem(hMain, COMBO_MAC);
	SendMessage(comboMAC, CB_GETLBTEXT, (int)SendMessage(comboMAC, CB_GETCURSEL, 0, 0), (LPARAM)mac_get_combo);

	PIP_ADAPTER_INFO pAdapterInfo, pAdapter;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
    {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
     }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
    {
        pAdapter = pAdapterInfo;
        while (pAdapter) 
		{
			LPWSTR unicodestr = Unicode_Cast(pAdapter->Description);
			//unicodestr [wcslen(unicodestr) - 27] = NULL;
			
			if(wcscmp(mac_get_combo, unicodestr) == 0)
			{
				WCHAR mac[200];
				swprintf_s(mac, 200, L"%02X-%02X-%02X-%02X-%02X-%02X\n", pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2], pAdapter->Address[3], 
																												pAdapter->Address[4], pAdapter->Address[5]);
				//MessageBox(0, mac, 0, 0);
				WCHAR MacCon[200] = L"MAC Address\t: ";
				wcscat_s (MacCon, 200, mac);
				ST3 = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", MacCon, WS_CHILD, 26, 280, 220, 20, hMain, 0, hIns, 0);
				SendMessage(ST3, WM_SETFONT, (WPARAM)hFont, 0);
				ShowWindow(ST3, SW_SHOWNORMAL);
			}
			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);
}
void MAC_SPOOF_CHK (HWND hMain)
{
	wchar_t mac_get_combo[200];
	LPWSTR macID;
	int i = 0;
	HKEY hNetCards, hQury;
	wchar_t keyNameBuf[512], NetCardName[260], Exist[260], copyStat[80];
	DWORD BufSiz = 512;
	
	comboMAC = GetDlgItem(hMain, COMBO_MAC);
	SendMessage(comboMAC, CB_GETLBTEXT, (int)SendMessage(comboMAC, CB_GETCURSEL, 0, 0), (LPARAM)mac_get_combo);

	PIP_ADAPTER_INFO pAdapterInfo, pAdapter;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
    {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
     }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
    {
        pAdapter = pAdapterInfo;
        while (pAdapter) 
		{
			LPWSTR unicodestr = Unicode_Cast(pAdapter->Description);
			//unicodestr [wcslen(unicodestr) - 27] = NULL;
			
			if(wcscmp(mac_get_combo, unicodestr) == 0)
			{
				macID = Unicode_Cast(pAdapter->AdapterName);
				break;
			}
			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);

	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}", 0, KEY_READ, &hNetCards);
	while (RegEnumKeyEx(hNetCards, i++, keyNameBuf, &BufSiz, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS) 
	{
		RegOpenKeyEx(hNetCards, keyNameBuf, 0, KEY_READ, &hQury);
		BufSiz = 512;
		crap = 0;
		RegQueryValueEx(hQury, L"NetCfgInstanceId", 0, &crap, (LPBYTE)NetCardName, &BufSiz);
		if (wcscmp(NetCardName, macID) == 0)
		{
			crap =0;
			BufSiz = 512;
			if (RegQueryValueEx(hQury, L"NetworkAddress", 0, &crap, (LPBYTE)Exist, &BufSiz) == ERROR_SUCCESS)
				swprintf_s(copyStat, 80, L"%s", L"Yes");
			else
				swprintf_s(copyStat, 80, L"%s", L"No");
		}
		RegCloseKey(hQury);
	}
	BufSiz = 512;
	RegCloseKey (hNetCards);
	WCHAR Spoof[80] = L"Spoofed\t: ";
	wcscat_s (Spoof, 80, copyStat);
	ST4 = CreateWindowEx(WS_EX_TRANSPARENT, L"STATIC", Spoof, WS_CHILD, 26, 297, 180, 20, hMain, 0, hIns, 0);
	SendMessage(ST4, WM_SETFONT, (WPARAM)hFont, 0);
	ShowWindow(ST4, SW_SHOWNORMAL);

}



void MAIN_BUTTON_SPOOFMAC (HWND hMain)										// adapter >> connec retrv
{
	wchar_t BufferS_sp[300];
	LPWSTR hID;
	SendMessage(comboMAC, CB_GETLBTEXT, (int)SendMessage(comboMAC, CB_GETCURSEL, 0, 0), (LPARAM)BufferS_sp);
	wcscpy_s(BufferS_SP_copy, 300, BufferS_sp);

	PIP_ADAPTER_INFO pAdapterInfo, pAdapter;																	
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
    {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
     }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
    {
        pAdapter = pAdapterInfo;
        while (pAdapter) 
		{
			LPWSTR unicodestr2 = Unicode_Cast(pAdapter->Description);
			//unicodestr2 [wcslen(unicodestr2) - 27] = NULL;
			
			if(wcscmp(BufferS_sp, unicodestr2) == 0)
			{
				hID = Unicode_Cast(pAdapter->AdapterName);
				break;
			}
			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);

	HKEY hNetworks, hCnn;
	int i=0;
	wchar_t keyNameBuf_sp[512], Connc_re_sp[200];
	DWORD BufSiz = 512;
	DWORD Buf = 200;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}", 0, KEY_READ, &hNetworks);
	while (RegEnumKeyEx(hNetworks, i++, keyNameBuf_sp, &BufSiz, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS) 
	{
		if(wcscmp(keyNameBuf_sp, hID)==0)
		{
			wcscat_s(keyNameBuf_sp, 512, L"\\Connection");
			RegOpenKeyEx(hNetworks, keyNameBuf_sp, 0, KEY_READ, &hCnn);
			crap =0;
			RegQueryValueEx(hCnn, L"Name", 0, &crap, (LPBYTE)Connc_re_sp, &Buf);
			wcscpy_s(Connec_spMAC, 300, Connc_re_sp);
		}
		Buf = 200;
		BufSiz = 512;
		RegCloseKey (hCnn);
	}
	RegCloseKey(hNetworks);
	
	DialogBox(hIns, MAKEINTRESOURCE(IDD_SPMAC), hMain, (DLGPROC)SpoofMACDlg);
}
void SP_SETMAC_BTN_MAC_CHK (HWND hspMAC)
{
	GetDlgItemText(hspMAC, ID_EDIT1, t1, sizeof(t1));
	GetDlgItemText(hspMAC, ID_EDIT2, t2, sizeof(t2));
	GetDlgItemText(hspMAC, ID_EDIT3, t3, sizeof(t3));
	GetDlgItemText(hspMAC, ID_EDIT4, t4, sizeof(t4));
	GetDlgItemText(hspMAC, ID_EDIT5, t5, sizeof(t5));
	GetDlgItemText(hspMAC, ID_EDIT6, t6, sizeof(t6));
	
	wcscat_s(t1, 120, t2);
	wcscat_s(t1, 120, t3);
	wcscat_s(t1, 120, t4);
	wcscat_s(t1, 120, t5);
	wcscat_s(t1, 120, t6);
	
	if (IsValidMAC(t1) == false)
	{
		MessageBox(hspMAC, L"You have entered an Invalid MAC", L"Invalid Input", MB_ICONERROR | MB_OK);
		SetFocus(GetDlgItem (hspMAC, ID_EDIT1));
		SendDlgItemMessage(hspMAC, ID_EDIT1, EM_SETSEL, 0, -1);
	}
	else if (IsValidMAC(t1) == true)
		SP_SETMAC_BTN (hspMAC);
}
void SP_SETMAC_BTN (HWND hspMAC)
{
	wchar_t mac_get_combo[200];
	LPWSTR macID;
	int i = 0;
	HKEY hNetCards, hQury;
	wchar_t keyNameBuf[512], NetCardName[260];
	DWORD BufSiz = 512;
	
	comboMAC = GetDlgItem(hMainClone, COMBO_MAC);
	SendMessage(comboMAC, CB_GETLBTEXT, (int)SendMessage(comboMAC, CB_GETCURSEL, 0, 0), (LPARAM)mac_get_combo);
	
	PIP_ADAPTER_INFO pAdapterInfo, pAdapter;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
    {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
     }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
    {
        pAdapter = pAdapterInfo;
        while (pAdapter) 
		{
			LPWSTR unicodestr = Unicode_Cast(pAdapter->Description);
			//unicodestr [wcslen(unicodestr) - 27] = NULL;
			
			if(wcscmp(mac_get_combo, unicodestr) == 0)
			{
				macID = Unicode_Cast(pAdapter->AdapterName);
				break;
			}
			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);

	HKEY hNetworks, hCnn;
	int i2=0;
	wchar_t keyNameBuf_sp[512], Connc_re_sp[200];
	DWORD BufSize2 = 512;
	DWORD Buf = 200;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}", 0, KEY_READ, &hNetworks);
	while (RegEnumKeyEx(hNetworks, i2++, keyNameBuf_sp, &BufSize2, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS) 
	{
		if(wcscmp(keyNameBuf_sp, macID)==0)
		{
			wcscat_s(keyNameBuf_sp, 512, L"\\Connection");
			RegOpenKeyEx(hNetworks, keyNameBuf_sp, 0, KEY_READ, &hCnn);
			crap =0;
			RegQueryValueEx(hCnn, L"Name", 0, &crap, (LPBYTE)Connc_re_sp, &Buf);
			RegCloseKey (hCnn);
			break;
		}
		Buf = 200;
		BufSize2 = 512;
	}
	RegCloseKey(hNetworks);


	DWORD BufSiz1 = 512;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}", 0, KEY_READ, &hNetCards);
	while (RegEnumKeyEx(hNetCards, i++, keyNameBuf, &BufSiz1, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS) 
	{
		RegOpenKeyEx(hNetCards, keyNameBuf, 0, KEY_READ | KEY_WRITE, &hQury);
		BufSiz1 = 512;
		crap = 0;
		RegQueryValueEx(hQury, L"NetCfgInstanceId", 0, &crap, (LPBYTE)NetCardName, &BufSiz1);
		if (wcscmp(NetCardName, macID) == 0)
		{
			if (RegSetValueEx(hQury, L"NetworkAddress", 0, REG_SZ, (const BYTE*)t1, sizeof(wchar_t) * ((DWORD)wcslen(t1) + 1)) == ERROR_SUCCESS)
			{
				HWND SL_SPMAC = CreateDialog(hIns, MAKEINTRESOURCE(SLEEP_SPMAC), hspMAC, 0);
				ShowWindow(SL_SPMAC, SW_SHOWNORMAL);
				UpdateWindow (SL_SPMAC);			
				Sleep(900);
				DestroyWindow (SL_SPMAC);

				HWND Sleep_Adap = CreateDialog(hIns, MAKEINTRESOURCE(SLEEP_READAP), hspMAC, 0);
				ShowWindow(Sleep_Adap, SW_SHOWNORMAL);
				UpdateWindow(Sleep_Adap);
				Adapter_Restart(Connc_re_sp);
				Sleep(400);
				DestroyWindow(Sleep_Adap); 
				
				MessageBox(hspMAC, L"MAC Address Spoofed Successfully", L"MAC Adreess Spoofed", MB_ICONINFORMATION | MB_OK);
			}
			else
				MessageBox(hspMAC, L"Not enough rights to modify windows registry. Check system rights.", L"Security Denies", MB_ICONERROR | MB_OK);
		}
		RegCloseKey(hQury);
	}
	BufSiz1 = 512;
	RegCloseKey (hNetCards);
}
void MAIN_RESTORE_MAC_YES_NO (HWND hMain)
{
	if (MessageBox(hMain, L"Do you really want to restore original MAC address ?", L"Confirm", MB_ICONQUESTION | MB_YESNO) == IDYES)
		MAIN_RESTORE_MAC (hMain);		
}
void MAIN_RESTORE_MAC (HWND hMain)
{
	wchar_t mac_get_combo[200];
	LPWSTR macID;
	int i = 0;
	HKEY hNetCards, hQury;
	wchar_t keyNameBuf[512], NetCardName[260];
	DWORD BufSiz = 512;
	
	comboMAC = GetDlgItem(hMain, COMBO_MAC);
	SendMessage(comboMAC, CB_GETLBTEXT, (int)SendMessage(comboMAC, CB_GETCURSEL, 0, 0), (LPARAM)mac_get_combo);

	PIP_ADAPTER_INFO pAdapterInfo, pAdapter;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));

    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
    {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);
     }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
    {
        pAdapter = pAdapterInfo;
        while (pAdapter) 
		{
			LPWSTR unicodestr = Unicode_Cast(pAdapter->Description);
			//unicodestr [wcslen(unicodestr) - 27] = NULL;
			
			if(wcscmp(mac_get_combo, unicodestr) == 0)
			{
				macID = Unicode_Cast(pAdapter->AdapterName);
				break;
			}
			pAdapter = pAdapter->Next;
		}
	}
	if (pAdapterInfo)
		FREE(pAdapterInfo);

	HKEY hNetworks, hCnn;
	int i2=0;
	wchar_t keyNameBuf_sp[512], Connc_re_sp[200];
	DWORD BufSize2 = 512;
	DWORD Buf = 200;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Network\\{4D36E972-E325-11CE-BFC1-08002BE10318}", 0, KEY_READ, &hNetworks);
	while (RegEnumKeyEx(hNetworks, i2++, keyNameBuf_sp, &BufSize2, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS) 
	{
		if(wcscmp(keyNameBuf_sp, macID)==0)
		{
			wcscat_s(keyNameBuf_sp, 512, L"\\Connection");
			RegOpenKeyEx(hNetworks, keyNameBuf_sp, 0, KEY_READ, &hCnn);
			crap =0;
			RegQueryValueEx(hCnn, L"Name", 0, &crap, (LPBYTE)Connc_re_sp, &Buf);
			RegCloseKey (hCnn);
			break;
		}
		Buf = 200;
		BufSize2 = 512;
	}
	RegCloseKey(hNetworks);


	DWORD BufSiz1 = 512;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}", 0, KEY_READ, &hNetCards);
	while (RegEnumKeyEx(hNetCards, i++, keyNameBuf, &BufSiz1, 0, NULL, NULL, &writtenTime) == ERROR_SUCCESS) 
	{
		RegOpenKeyEx(hNetCards, keyNameBuf, 0, KEY_READ | KEY_SET_VALUE, &hQury);
		BufSiz1 = 512;
		crap = 0;
		RegQueryValueEx(hQury, L"NetCfgInstanceId", 0, &crap, (LPBYTE)NetCardName, &BufSiz1);
		if (wcscmp(NetCardName, macID) == 0)
		{
			if (RegDeleteValue(hQury, L"NetworkAddress") == ERROR_SUCCESS)
			{
				HWND RE_SPMAC = CreateDialog(hIns, MAKEINTRESOURCE(SLEEP_REMAC), hMain, 0);
				ShowWindow(RE_SPMAC, SW_SHOWNORMAL);
				UpdateWindow (RE_SPMAC);			
				Sleep(900);
				DestroyWindow (RE_SPMAC);

				HWND Sleep_Adap = CreateDialog(hIns, MAKEINTRESOURCE(SLEEP_READAP), hMain, 0);
				ShowWindow(Sleep_Adap, SW_SHOWNORMAL);
				UpdateWindow(Sleep_Adap);
				Adapter_Restart(Connc_re_sp);
				Sleep(400);
				DestroyWindow(Sleep_Adap); 

				MessageBox(hMain, L"Original MAC Address Restored Successfully.", L"Restore Complete", MB_ICONINFORMATION | MB_OK);
			}
			else
				MessageBox(hMain, L"MAC Address has already set to it's original one.", L"Not Spoofed", MB_ICONEXCLAMATION | MB_OK);
		}
		RegCloseKey(hQury);
	}
	BufSiz1 = 512;
	RegCloseKey (hNetCards);
}

void SP_RAND_MAC (HWND hspMAC)
{
	wchar_t rand2[20], rand3[20], rand4[20], rand5[20], rand6[20];
	swprintf_s(rand2, 20, L"%X", rand() & 0xFF);
	swprintf_s(rand3, 20, L"%X", rand() & 0xFF);
	swprintf_s(rand4, 20, L"%X", rand() & 0xFF);
	swprintf_s(rand5, 20, L"%X", rand() & 0xFF);
	swprintf_s(rand6, 20, L"%X", rand() & 0xFF);

	SetDlgItemText(hspMAC, ID_EDIT1, L"00");
	SetDlgItemText(hspMAC, ID_EDIT2, rand2);
	SetDlgItemText(hspMAC, ID_EDIT3, rand3);
	SetDlgItemText(hspMAC, ID_EDIT4, rand4);
	SetDlgItemText(hspMAC, ID_EDIT5, rand5);
	SetDlgItemText(hspMAC, ID_EDIT6, rand6);
}
void RE_DEL_ALL (HWND hReIP)
{
	wchar_t WinDel[MAX_PATH], Connec_Retrv2[100];

	HWND sleepW = CreateDialog(hIns, MAKEINTRESOURCE(SLEEP_REDE), hReIP, 0);
	ShowWindow(sleepW, SW_SHOWNORMAL);
	UpdateWindow(sleepW);

	GetWindowText(hReIP, Connec_Retrv2, 100);	

	GetWindowsDirectory(WinDel, MAX_PATH);
	wcscat_s(WinDel, MAX_PATH, L"\\IPMAC\\");
	wcscat_s(WinDel, MAX_PATH, Connec_Retrv2);
	DeleteAllFiles(WinDel);
	Sleep(1000);
	EndDialog(sleepW, 0);	

	if (RemoveDirectory(WinDel) != 0)
		MessageBox(hReIP, L"All backups cleared successfully.", L"Success", MB_ICONINFORMATION | MB_OK);
	else
		MessageBox(hReIP, L"Error deleting backups.", L"Error", MB_ICONERROR | MB_OK);	

}	

void RE_DEL_SELECTED (HWND hReIP)
{
	HWND hWait = CreateDialog(hIns, MAKEINTRESOURCE(SLEEP_REDE), hReIP, 0);
	SetDlgItemText(hWait, SLEEP_RE_DE, L"Deleting selected IP configuration..");
	ShowWindow(hWait, SW_SHOWNORMAL);
	UpdateWindow(hWait);

	wchar_t Connec_Retrv[100], WinDir[MAX_PATH], ReadFilePath[MAX_PATH], WinDirBak[MAX_PATH];
	wchar_t ReadRadio[1024], ReadRadioBak[1024], ReadRadioMsg[400];
	wchar_t Delete_Msg [80] = L" IP deleted successfully";
	int reserve, length;				
	for(int j=0; j<8; j++) 	{
		if (SendMessage( hRadio[j], BM_GETCHECK, 0, 0 ) == BST_CHECKED) 	{
			reserve = j;
			break;
		}
	}
	SendMessage(hRadio[reserve], WM_GETTEXT, (WPARAM)1024, (LPARAM)ReadRadio);
	wcscpy_s(ReadRadioBak, 1024, ReadRadio);				//

	wistringstream stream(ReadRadio);
	stream.getline(ReadRadio, sizeof(ReadRadio), ' ');
	
	wcscpy_s(ReadRadioMsg, 400, ReadRadio);
	wcscat_s(ReadRadioMsg, 400, Delete_Msg);
	
	GetWindowText (hReIP, Connec_Retrv, 100);
	
	GetWindowsDirectory(WinDir, MAX_PATH);
	wcscat_s(WinDir, MAX_PATH, L"\\IPMAC\\");
	wcscat_s(WinDir, MAX_PATH, Connec_Retrv);
	wcscat_s(WinDir, MAX_PATH, L"\\");
	wcscpy_s(WinDirBak, sizeof(WinDir), WinDir);
	wcscpy_s(ReadFilePath, MAX_PATH, WinDir);
	wcscat_s(ReadFilePath, MAX_PATH, L"log.txt");
	wcscat_s(ReadRadio, 400, L".DAT");
	wcscat_s(WinDir, MAX_PATH, ReadRadio);	
	
	//MessageBox(hReIP, WinDirBak, 0, 0);
	DeleteFile(WinDir);		//file deleted.. entry have to be deleted.
	
	LPCSTR lineSt;		string line;		//wchar_t lineW[1024];

	ifstream inFile (ReadFilePath);
	ofstream ofFile ("tmp.txt");
	
	while( getline(inFile, line) )		{
		lineSt = line.c_str();																	// string    to    const char*
		
		//MessageBox(hReIP, (LPCWSTR)Unicode_Cast((LPSTR)lineSt), 0, 0);

		if(wcscmp((LPCWSTR)Unicode_Cast((LPSTR)lineSt), ReadRadioBak)!=0)
            ofFile << line << "\n";
    }
	inFile.close();
    ofFile.close();    
	DeleteFile(ReadFilePath);
	_wrename(L"tmp.txt", ReadFilePath);

	ifstream filestr(ReadFilePath);
	filestr.seekg(0, ios::end); 
	length = filestr.tellg();
	filestr.close(); 

	if ( length == 0 )	{
		DeleteFile(ReadFilePath);
		RemoveDirectory(WinDirBak);
	}
	Sleep(1400);
	EndDialog(hWait, 0);
	MessageBox(hReIP, ReadRadioMsg, L"Config deleted", MB_OK);
}

void Splash_FirstRun()	{
	wchar_t WinDirec[MAX_PATH];
	GetWindowsDirectory(WinDirec, MAX_PATH);							//directory create
	wcscat_s(WinDirec, 260, L"\\IPMAC");

	if (_waccess(WinDirec, 0) != 0)	
	{
		_wrename(L".\\bin.ipm", L".\\splash.bmp");
		CSplash splash1(L".\\splash.bmp", RGB(128, 128, 128));
		splash1.ShowSplash();
		Sleep(3000); 
		splash1.CloseSplash();
		_wrename(L".\\splash.bmp", L".\\bin.ipm");
		CreateDirectory(WinDirec, 0);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Splash_FirstRun();
	//InitCommonControls();
	hIns = hInstance;
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), 0, (DLGPROC)MainDlgProc);
	return 0;
}