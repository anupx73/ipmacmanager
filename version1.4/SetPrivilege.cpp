#include "SetPrivilege.h"

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
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
