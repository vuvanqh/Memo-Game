#pragma once
#include <windows.h>
#include <string>
#include "board.h"
#include <chrono>
#include <cstdlib>

class memo_app
{
private:
	HWND m_main;
	BOOL registerClass();
	HWND createWindow();
	HINSTANCE m_instance;
	HBRUSH m_brush;
	board m_board;
	static std::wstring const class_name;
	std::vector<HWND> tiles;
	static constexpr UINT_PTR s_timer = 1;
	std::chrono::time_point<std::chrono::system_clock> m_startTime;
	std::vector<int>sequence;
	std::vector<int>guess;
	int score = 0;
	int n;
	int count = 0;
	BOOL input = false;

	static LRESULT CALLBACK winproc_static(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	LRESULT winproc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
	void onTimer();
	void generateSequence()
	{
		srand(time(0));
		sequence.push_back(rand() % (n * n));
	}
	void game();
	void showSequence();
	void pressButton(int x, int y);
public:
	int run(int showCommand);
	memo_app(HINSTANCE instance, int n);
};