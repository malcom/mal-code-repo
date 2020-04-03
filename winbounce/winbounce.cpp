//
// Bounces your windows on the screen! Made for fun on April Fools' Day 2020 ;)
// http://blog.malcom.pl/2020/winbounce-poruszaj-oknami-na-ekranie-windowsa.html
//
// Copyright (C) 2020 by Marcin 'Malcom' Malich <me@malcom.pl>
// Released under the MIT License
//
// cl /EHsc /MT /GL /O2 /W4 /std:c++17 winbounce.cpp user32.lib
//

//
// TODO:
// - maybe add some error-handling stuff ;)
// - s<id> support / all on all desktop
//

#include <atomic>
#include <random>
#include <string_view>
#include <cstdio>
#include <cstdlib>

#include <windows.h>

// some helpers

inline int GenRandom() {

	// as global without cluttering the global namespace ;)
	static std::mt19937 gen(std::random_device{}());
	static std::uniform_int_distribution<> dis(-25, 25);

	return dis(gen);
}

inline RECT GetDesktopRect() {
	RECT rect;
	::GetWindowRect(::GetDesktopWindow(), &rect);
	return rect;
}

inline void SetWindowPos(HWND hWnd, LONG cx, LONG cy) {

	::SetWindowPos(
		hWnd,
		HWND_TOP,
		cx,
		cy,
		0,
		0,
		SWP_NOZORDER | SWP_NOSIZE
	);
}


// atomic as the system creates a new thread to execute the CtrlHandler,
std::atomic_bool process = true;

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {

	switch (fdwCtrlType) {

		case CTRL_C_EVENT:
		case CTRL_BREAK_EVENT:
		case CTRL_CLOSE_EVENT:
			process = false;
			std::printf("Break signal received...\n");
			return TRUE;

		default:
			return FALSE;
	}
}


struct Param {

	enum Types {
		Unknown = 0,
		Win,
		Proc,
		Screen,
		All,
	};

	Types Type = Unknown;
	unsigned int Value = 0;
};

Param ParseParam(int argc, char* argv[]) {

	if (argc != 2)
		return {};

	std::string_view arg(argv[1]);
	if (arg.size() < 1)
		return {};

	if (arg == "all")
		return { Param::All, 0 };

	Param param;
	if (arg.front() == 'w') {
		param.Type = param.Win;
		arg.remove_prefix(1);

	} else if (arg.front() == 'p') {
		param.Type = param.Proc;
		arg.remove_prefix(1);

	} else if (arg.front() == 's') {
		param.Type = param.Screen;
		arg.remove_prefix(1);

	} else {
		// default: <hwnd>
		param.Type = param.Win;
	}

	// only strto* funcs support auto-detection of the numeric base
	param.Value = std::strtoul(arg.data(), nullptr, 0);

	return param;
}


using Hwnds = std::vector<HWND>;

struct EnumWindowsProcContext {
	const Param& Param;
	Hwnds& Hwnds;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {

	// omit invalid and invisible windows
	if (!::IsWindow(hwnd) || !::IsWindowVisible(hwnd))
		return TRUE;

	auto ctx = reinterpret_cast<EnumWindowsProcContext*>(lParam);

	if (ctx->Param.Type == Param::Proc) {

		DWORD pid;
		::GetWindowThreadProcessId(hwnd, &pid);

		if (pid == ctx->Param.Value)
			ctx->Hwnds.push_back(hwnd);

	} else {
		// screen or all
		ctx->Hwnds.push_back(hwnd);
	}

	return TRUE;
}

Hwnds GrabWindows(const Param& param) {

	Hwnds hwnds;
	EnumWindowsProcContext ctx{ param, hwnds };

	switch (param.Type) {

		case Param::Win:
			// push the hwnd by EnumProc to validate
			EnumWindowsProc(reinterpret_cast<HWND>(param.Value), reinterpret_cast<LPARAM>(&ctx));
			break;

		case Param::Proc:
		case Param::Screen:
		case Param::All:
			::EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&ctx));
			break;

		default:
			;
	}

	return hwnds;
}


struct Window {
	HWND hWnd;

	POINT OldPosition;
	UINT OldShowState;

	RECT Rect;
	POINT Vel;
};

void PrepareWindow(HWND hWnd, Window& win) {

	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	::GetWindowPlacement(hWnd, &wndpl);

	win.hWnd = hWnd;
	win.OldPosition.x = wndpl.rcNormalPosition.left;
	win.OldPosition.y = wndpl.rcNormalPosition.top;
	win.OldShowState = wndpl.showCmd;
	win.Rect = wndpl.rcNormalPosition;
	win.Vel.x = GenRandom();
	win.Vel.y = GenRandom();

	if (win.OldShowState != SW_NORMAL && win.OldShowState != SW_RESTORE)
		::ShowWindow(hWnd, SW_RESTORE);
}

void MoveWindow(Window& win, const RECT& screen) {

	win.Rect.left += win.Vel.x;
	win.Rect.right += win.Vel.x;
	win.Rect.top += win.Vel.y;
	win.Rect.bottom += win.Vel.y;

	if (win.Rect.left <= screen.left || win.Rect.right >= screen.right)
		win.Vel.x *= -1;
	if (win.Rect.top <= screen.top || win.Rect.bottom >= screen.bottom)
		win.Vel.y *= -1;

	SetWindowPos(win.hWnd, win.Rect.left, win.Rect.top);
}

void RestoreWindow(Window& win) {

	SetWindowPos(win.hWnd, win.OldPosition.x, win.OldPosition.y);

	if (win.OldShowState != SW_NORMAL && win.OldShowState != SW_RESTORE)
		::ShowWindow(win.hWnd, win.OldShowState);
}


int main(int argc, char* argv[]) {

	const Param param = ParseParam(argc, argv);
	if (param.Type == Param::Unknown) {

		std::puts("Bounces your windows on the screen!");
		std::puts("");
		std::puts("Usage: winbounce <param>");
		std::puts("Available param value:");
		std::puts(" <hwnd>   like w<hwnd>");
		std::puts(" w<hwnd>  only the specified window");
		std::puts(" p<pid>   all windows in the specified process");
		std::puts(" s<id>    all windows in the specified screen / not supported yet /");
		std::puts(" all      all windows / only on default screen now /");
		std::puts("");
		std::puts("Copyrigth (C) 2020 by Marcin 'Malcom' Malich <me@malcom.pl>");
		std::puts("Released under the MIT License");
		std::puts("");
		std::puts("Made for fun on April Fools' Day 2020 ;)");

		return 1;
	}

	if (!::SetConsoleCtrlHandler(CtrlHandler, TRUE)) {
		std::printf("Can't set the console handler!\n");
		return 1;
	}

	const RECT screen = GetDesktopRect();

	Hwnds hwnds = GrabWindows(param);
	std::printf("Found windows: %d\n", hwnds.size());

	if (hwnds.size() == 0) {
		std::printf("Not found any available / visible window to move...\n");
		return 1;
	}

	using Windows = std::vector<Window>;
	Windows wins;

	for (auto hwnd : hwnds) {
		Window win;
		PrepareWindow(hwnd, win);
		wins.emplace_back(win);
	}

	std::printf("Windows prepared\n");

	std::printf("Bouncing...\n");
	while (process) {

		for (auto& win : wins)
			MoveWindow(win, screen);

		::Sleep(50);
	}

	for (auto& win : wins)
		RestoreWindow(win);

	std::printf("Old windows state restored\n");

	return 0;
}