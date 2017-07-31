#include "DeleteAllFile.h"

void DeleteAllFiles(wchar_t* folderPath)
{
	wchar_t fileFound[256];
	WIN32_FIND_DATA info;
	HANDLE hp; 
	swprintf_s(fileFound, 256, L"%s\\*.*", folderPath);
	hp = FindFirstFile(fileFound, &info);
	while(FindNextFile(hp, &info)!=0 )
    {
       swprintf_s(fileFound, 256, L"%s\\%s", folderPath, info.cFileName);
       DeleteFile(fileFound);
	} 
	FindClose(hp);
}