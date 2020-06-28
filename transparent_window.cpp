#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <locale.h>
#include <dwmapi.h>
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1 : 0)

LPWSTR lpWindowName;
void cls(HANDLE hConsole)
{
	COORD coordScreen = {0,0};
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	DWORD dwConSize;
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
	FillConsoleOutputCharacter(hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten);
	GetConsoleScreenBufferInfo(hConsole, &csbi);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);
	SetConsoleCursorPosition(hConsole, coordScreen);
	return;
}
HWND hTarget;


HRESULT EnableBlurBehind(HWND hwnd, BOOL Enable)
{
   HRESULT hr = S_OK;

   // Create and populate the Blur Behind structure
   DWM_BLURBEHIND bb = {0};

   // Enable Blur Behind and apply to the entire client area
   bb.dwFlags = DWM_BB_ENABLE;
   bb.fEnable = Enable;
   bb.hRgnBlur = NULL;

   // Apply Blur Behind
   hr = DwmEnableBlurBehindWindow(hwnd, &bb);
   
   return hr;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	setlocale(LC_ALL, "chs");
	POINT p;
	lpWindowName = (LPWSTR)LocalAlloc(LMEM_ZEROINIT,  512 * 2);
	HANDLE hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	HWND hPrev = NULL;
	while(1){
		
		GetCursorPos(&p);
		
		hTarget = WindowFromPoint(p);
		if(!hTarget){
			Sleep(20);
			continue;
		}
		
		HDC hdc = GetDC(HWND_DESKTOP);
		RECT rc;
		GetWindowRect(hTarget, &rc);
		FrameRect(hdc, &rc, CreateSolidBrush(RGB(255, 0, 0)));
		GetWindowTextW(hTarget, lpWindowName, 512);
		if(hPrev != hTarget) {
			cls(hStdOutput);
			wprintf(L"当前选定窗口名:%s\n", lpWindowName);
			wprintf(L"按[Enter]确定窗口。\n");
		}
		if(KEY_DOWN(VK_RETURN)) break;
		Sleep(20);
		hPrev = hTarget;
	}
	LONG lWndLong = GetWindowLong(hTarget, GWL_EXSTYLE);
	SetWindowLong(hTarget, GWL_EXSTYLE, lWndLong|WS_EX_LAYERED);
	int per = 0;
	Sleep(200);
	cls(hStdOutput);
	
	SetLayeredWindowAttributes(hTarget, 0, 255, LWA_ALPHA);
	bool changed = true;
	bool AeroEnabled = false;
	while(1){
		if(changed){
			cls(hStdOutput);
			wprintf(L"当前已选定窗口:%s\n", lpWindowName);
			wprintf(L"请按\'a\'%s用毛玻璃效果，按↑↓键调节透明度:%d%%\n", AeroEnabled?L"禁":L"启", per);
			wprintf(L"按[Enter]确定设定。\n");
			changed = false;
		}
		
		if(KEY_DOWN('A')){
			AeroEnabled = !AeroEnabled;
			EnableBlurBehind(hTarget, AeroEnabled);
			Sleep(150);changed = true;
			continue;
		}
		
		if(KEY_DOWN(VK_UP)&&per+3<=100) {
			per+=3;
			SetLayeredWindowAttributes(hTarget, 0, 255*((float)(100-per)/100.0), LWA_ALPHA);
			changed = true;
			Sleep(50);
			continue;
		}
		if(KEY_DOWN(VK_DOWN)&&per-3>=0) {
			per-=3;
			SetLayeredWindowAttributes(hTarget, 0, 255*((float)(100-per)/100.0), LWA_ALPHA);
			changed = true;Sleep(50);
			continue;
		}
		if(KEY_DOWN(VK_RETURN)) break;
		Sleep(50);
	}
	
	
	return 0;
}
