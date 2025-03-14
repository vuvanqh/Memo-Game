#include "memory_app.h"
#include <algorithm>
#include <windowsx.h>
#include <wchar.h>
const std::wstring memo_app::class_name{ L"memo_game" };

BOOL memo_app::registerClass()
{
	WNDCLASSEX window{};
	if ((GetClassInfoExW(m_instance, class_name.c_str(), &window)) != 0) return true;

	window = {
		.cbSize = sizeof(WNDCLASSEX),
		.lpfnWndProc = winproc_static,
		.hInstance = m_instance,
		.hCursor = LoadCursorW(nullptr,L"IDC_ARROW"),
		.hbrBackground = CreateSolidBrush(RGB(255,253,208)),
		.lpszClassName = class_name.c_str()
	};
	return RegisterClassExW(&window) != 0;
}

HWND memo_app::createWindow()
{
	DWORD style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
		WS_BORDER | WS_MINIMIZEBOX;
	RECT size{ 0,0,m_board.size,m_board.size };
	AdjustWindowRectEx(&size, style, false, 0);
	HWND desktop = GetDesktopWindow();

	HWND window =  CreateWindowExW(
		0,
		class_name.c_str(),
		L"Press ESC to start!",
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION |
		WS_BORDER | WS_MINIMIZEBOX,
		(GetSystemMetrics(SM_CXSCREEN) - m_board.size)/2,
		(GetSystemMetrics(SM_CYSCREEN) - m_board.size)/2,
		size.right-size.left,
		size.bottom-size.top,
		0,
		0,
		m_instance,
		this
	);
	for (auto& f : m_board.fields())
	{
		tiles.push_back(CreateWindowExW(
			0, L"STATIC", nullptr,
			WS_CHILD | SS_CENTER,
			f.position.left, f.position.top,
			m_board.field_size, m_board.field_size,
			window, nullptr, m_instance, nullptr
		));
	}
	return window;
}

LRESULT memo_app::winproc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	memo_app* app = nullptr;
	if (message == WM_NCCREATE)
	{
		auto p = reinterpret_cast<LPCREATESTRUCTW>(lparam);
		app = static_cast<memo_app*>(p-> lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(app));
	}
	else
	{
		app = reinterpret_cast<memo_app*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	}
	if (app != nullptr)
	{
		return app-> winproc(window, message,
			wparam, lparam);
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

LRESULT memo_app::winproc(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CLOSE:
		DestroyWindow(window);
		return 0;
	case WM_CTLCOLORSTATIC:
		return reinterpret_cast<INT_PTR>(m_brush);
	case WM_TIMER:
		onTimer();
		return 0;
	case WM_KEYUP:
		if (wparam == VK_ESCAPE)
		{
			for (int i = 0; i < tiles.size(); i++)
				ShowWindow(tiles[i], SW_HIDE);
			SetWindowTextW(m_main, L"Score: 0 Memorize!");
			game();
		}
		return 0;
	case WM_LBUTTONUP:

		if (input)
			pressButton(GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam));
		return 0;
	}

	return DefWindowProcW(window, message, wparam, lparam);
}

memo_app::memo_app(HINSTANCE instance, int n)
	:m_instance{instance},m_board{board(n)},m_brush{CreateSolidBrush(RGB(124,10,2))},m_main{}, n{n}
{
	registerClass();
	m_main = createWindow();
}

int memo_app::run(int showCommand)
{
	ShowWindow(m_main, showCommand); 
	for (int i = 0; i < tiles.size(); i++)
		ShowWindow(tiles[i], SW_SHOWNA);

	m_startTime = std::chrono::system_clock::now();
	MSG msg{};
	BOOL result = true;
	while ((result = GetMessageW(&msg, m_main, 0, 0)) != 0)
	{
		if (result == -1)
			return EXIT_FAILURE;

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return EXIT_SUCCESS;
}

void memo_app::onTimer()
{
	wchar_t b[1024];
	if (guess.size() >= 1)
	{
		ShowWindow(tiles[guess[guess.size() - 1]], SW_HIDE);
		guess.clear();
		return;
	}
	if (count < sequence.size()) 
	{
		ShowWindow(tiles[sequence[std::max(0,count-1)]], SW_HIDE);
		ShowWindow(tiles[sequence[count]], SW_SHOWNA);
		count++;
	}
	else 
	{
		ShowWindow(tiles[sequence[count-1]], SW_HIDE);
		KillTimer(m_main, s_timer);
		input = true;
		wchar_t b[1024];
		swprintf(b, sizeof(b), L"Score: %d Guess!", score);
		SetWindowTextW(m_main, b);
	}
}

void memo_app::game()
{
	score = 0;
	input = false;
	generateSequence();
	showSequence();
}

void memo_app::showSequence() {
	input = false;
	count = 0;
	SetTimer(m_main, s_timer, 500, nullptr);
}

void memo_app::pressButton(int x, int y)
{
	//SetWindowTextW(m_main, L"clicked");
	for (int i = 0; i < tiles.size(); i++) 
	{
		RECT r{ x, y, x + 1, y + 1 };

		// Get the window rectangle of the tile in screen coordinates
		RECT r2;
		GetWindowRect(tiles[i], &r2);

		// Convert the screen coordinates of r2 to client coordinates
		ScreenToClient(m_main, (POINT*)&r2.left);  // Convert left, top
		ScreenToClient(m_main, (POINT*)&r2.right); // Convert right, bottom

		// Check if the two rectangles intersect
		if (IntersectRect(&r, &r, &r2))
		{
			guess.push_back(i);
			break;
		}
	}
	if (guess.size() > 1)
		ShowWindow(tiles[guess[guess.size() - 2]], SW_HIDE);
	ShowWindow(tiles[guess[guess.size() - 1]], SW_SHOWNA);
	if (guess.size() == sequence.size()) {
		if (guess == sequence) {
			wchar_t b[1024];
			swprintf(b, sizeof(b), L"Score: %d Memorize!", ++score);
			SetWindowTextW(m_main,b);
			generateSequence();
			showSequence();
		}
		else {
			SetWindowTextW(m_main,L"Wrong! ESC to restart!");
			for(int i=0;i<tiles.size();i++)
				ShowWindow(tiles[i], SW_SHOWNA);
			sequence.clear();
		}
	}
}