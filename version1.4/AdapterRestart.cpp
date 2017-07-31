#include "AdapterRestart.h"

void Adapter_Restart(LPWSTR connec_name)
{
	INetConnectionManager* pNet;
	INetConnection* pConn;
	IEnumNetConnection* pEnum;
	NETCON_PROPERTIES* pProps;
	wchar_t Temp[255];
	ULONG uCount;
	uCount = 0;

	swprintf_s(Temp, 255, L"%s", connec_name);
	CoInitialize(NULL);
	CoCreateInstance(CLSID_ConnectionManager, NULL, CLSCTX_SERVER, IID_INetConnectionManager, (void**)&pNet);
	pNet->EnumConnections(NCME_DEFAULT, &pEnum);

	while (pEnum->Next(1, &pConn, &uCount) == S_OK)
	{
		pConn->GetProperties( &pProps );
		if (!wcscmp(pProps->pszwName, Temp))
		{
			pConn->Disconnect();
			pConn->Connect();
			CoTaskMemFree(pProps->pszwName);
			CoTaskMemFree(pProps->pszwDeviceName);
			CoTaskMemFree(pProps);
			pConn->Release();
		}
	}
	pEnum->Release();
	pNet->Release();
	CoUninitialize();
}