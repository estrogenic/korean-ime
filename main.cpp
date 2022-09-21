#include "stdafx.h"
#include "Hangul.h"

void HideCursor()
{
	CONSOLE_CURSOR_INFO cursorInfo = { 0, };
	cursorInfo.dwSize = 1;
	cursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}
void cls()
{
	// Get the Win32 handle representing standard output.
	// This generally only has to be done once, so we make it static.
	static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	COORD topLeft = { 0, 0 };

	// std::cout uses a buffer to batch writes to the underlying console.
	// We need to flush that to the console because we're circumventing
	// std::cout entirely; after we clear the console, we don't want
	// stale buffered text to randomly be written out.
	std::cout.flush();

	// Figure out the current width and height of the console window
	if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
		// TODO: Handle failure!
		abort();
	}
	DWORD length = csbi.dwSize.X * csbi.dwSize.Y;

	DWORD written;

	// Flood-fill the console with spaces to clear it
	FillConsoleOutputCharacter(hOut, TEXT(' '), length, topLeft, &written);

	// Reset the attributes of every character to the default.
	// This clears all background colour formatting, if any.
	FillConsoleOutputAttribute(hOut, csbi.wAttributes, length, topLeft, &written);

	// Move the cursor back to the top left for the next sequence of writes
	SetConsoleCursorPosition(hOut, topLeft);
}

int main()
{
	std::setlocale(LC_ALL, "ko_KR.UTF-8");
	HideCursor();
	
	// 한글 입력 관련 변수
	std::wstring strContext;
	Hangul *hg = new Hangul;

	while (true)
	{
		auto c = _getch();

		if (c == 0xE0)
		{
			c = _getch();
			if (c == 0x4B)
			{
				// 커서를 좌측으로 이동
				hg->moveCursorTo((hg->nCursorPos) - 1, true);
				
				// 커서를 넣어서 출력
				std::wstring strNewContext = strContext;
				strNewContext.insert(hg->nCursorPos, 1, '|');
				cls();
				wprintf(L"%s", strNewContext.data());
				
				continue;
			}
			else if (c == 0x4D)
			{
				// 커서를 우측으로 이동
				hg->moveCursorTo((hg->nCursorPos) + 1, true, strContext.length());
				
				// 커서를 넣어서 출력
				std::wstring strNewContext = strContext;
				strNewContext.insert(hg->nCursorPos, 1, '|');
				cls();
				wprintf(L"%s", strNewContext.data());
				continue;
			}
		}

		std::wstring strNewContext = hg->AssembleHangul(strContext, c);
		if (strNewContext == strContext)
			continue;

		strContext = strNewContext;
		strNewContext.insert(hg->nCursorPos, 1, '|');
		cls();
		wprintf(L"%s", strNewContext.data());
	}
	
	delete hg;
	return 0;
}