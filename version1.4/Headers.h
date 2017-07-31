#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif

#include <windows.h>
#include <netcon.h>
#include <stdio.h>
#include <io.h>
#include <fstream>
#include <time.h>
#include <iphlpapi.h>
#include <stdlib.h>
#include <commctrl.h>
#include <string>
#include <sstream>

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment (lib, "ComCtl32.lib")
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;