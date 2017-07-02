#include "UnicodeCast.h"

LPWSTR Unicode_Cast(LPSTR ansistring)
{
	char *ansistr = ansistring;
	int a = lstrlenA(ansistr);
	LPWSTR unicodestr = SysAllocStringLen(NULL, a);
	MultiByteToWideChar(CP_ACP, 0, ansistr, a, unicodestr, a);
	return unicodestr;
}