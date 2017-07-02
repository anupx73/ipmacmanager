#include "splash.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CSplash splash1(L".\\ip_mac_splash.bmp", 0);
	splash1.ShowSplash();
	Sleep(3000); 
	splash1.CloseSplash();
	return 0;
}
