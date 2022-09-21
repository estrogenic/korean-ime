#include "stdafx.h"
#include "Hangul.h"

Hangul::Hangul()
{
	// 이전 한글을 저장할 변수를 힙에 동적으로 할당
	ppPrevLetter = new char* [5];
	for (int i = 0; i < 5; i++)
		ppPrevLetter[i] = new char[5]();
	cTempLetter = new char[2];
}

Hangul::~Hangul()
{
	// 힙에 동적으로 할당한 변수를 해제
	for (int i = 0; i < 5; i++)
		delete[] ppPrevLetter[i];
	delete[] ppPrevLetter;
	delete[] cTempLetter;
}

inline void Hangul::clearTempHangul(){ 	std::fill(nHangulCode, nHangulCode + 5, -1); }
inline void Hangul::strBackspace(std::wstring& s) { s.pop_back(); };
inline wchar_t Hangul::strGetLastChar(std::wstring& s) { return s.back(); };
inline void Hangul::charToStr(char* cc, char& c) { sprintf_s(cc, 2, "%c", c); }

void Hangul::moveCursorTo(int index, bool isUserInteract, size_t maxLength)
{
	// 최대 길이는 커서를 오른쪽으로 이동할 때 넣어줘야 함(문자열 길이보다 커지지 않도록)
	
	if (index < 0)
		return;
	
	if (isUserInteract)
		clearTempHangul();
	
	if (unsigned(index) > maxLength)
		return;
	
	nCursorPos = index;

	std::wstring aaaa = L"현재 커서 위치: ";
	aaaa.append(std::to_wstring(nCursorPos));
	SetWindowText(GetConsoleWindow(), aaaa.c_str());
}

int Hangul::findHangulByStr(uint8_t flagHangul, const char* strLetter)
{
	int nTempLetter = -1;
	switch (flagHangul)
	{
	case 0:
		nTempLetter = arrayIndexOf(HANGUL_CHO, strLetter);
		break;
	case 1:
		nTempLetter = arrayIndexOf(HANGUL_JUNG, strLetter);
		break;
	case 2:
		nTempLetter = arrayIndexOf(HANGUL_JONG, strLetter);
		break;
	}
	return nTempLetter;
}
template <typename T, size_t size> int Hangul::arrayIndexOf(T const (&array)[size], T const c)
{
	for (size_t i = 0; i < size; i++)
	{
		if (strcmp(array[i], c) == 0)
			return (int)i;
	}
	return -1;
}

std::wstring Hangul::AssembleHangul(std::wstring strCurrentContext, char c)
{
	if ('\n' == c)
		return strCurrentContext;

	if ('\r' == c)
		return strCurrentContext;
	
	// 커서 전후로 문자를 나눈다.
	std::wstring strRet = strCurrentContext.substr(0, nCursorPos);
	std::wstring testad = strCurrentContext.substr(nCursorPos);

	if ('\b' == c)
	{
		if (strRet.length() >= 1)
		{
			strBackspace(strRet);
			
			if (isEditing && usedAlphabet > 1)
			{
				// 에디팅 상태라면 자모음을 하나씩 지워주는 부분
				wchar_t wcTempHangul = 0;
				std::string tempStr;
				int tempIndex = 0;
				
				switch (usedAlphabet)
				{
				case 2:
					// 2개가 조합되었었음(초성+중성) -> 초성만 남기면 됨
					nHangulCode[1] = -1;
					wcTempHangul = HANGUL_KEYMAP[ppPrevLetter[3][0]];
					break;
				case 3:
					// 3개가 조합되었었음(초성+겹중성, 초성+중성+종성) -> 초성+중성만 남기면 됨
					nHangulCode[0] = findHangulByStr(0, ppPrevLetter[2]) * 21 * 28;
					nHangulCode[1] = findHangulByStr(1, ppPrevLetter[3]) * 28;  // 이후 겹종성 조합을 위해서 초기화
					nHangulCode[2] = -1;
					wcTempHangul = 0xAC00 + nHangulCode[0] + nHangulCode[1];
					
					break;
				case 4:
					// 4개가 조합됬었음(초성+겹중성+종성, 초성+중성+겹종성) -> 초성+겹중성, 초성+중성+종성
					nHangulCode[3] = -1;
					
					// 겹중성 조합 가능 여부 확인
					tempStr = ppPrevLetter[2];
					tempStr.append(ppPrevLetter[3]);
					tempIndex = findHangulByStr(1, tempStr.c_str());
					
					if (tempIndex != -1)
						wcTempHangul = 0xAC00 + (findHangulByStr(0, ppPrevLetter[1]) * 21 * 28) + (tempIndex * 28);  // 겹중성	
					else
						wcTempHangul = 0xAC00 + (findHangulByStr(0, ppPrevLetter[1]) * 21 * 28) + (findHangulByStr(1, ppPrevLetter[2]) * 28) + (findHangulByStr(2, ppPrevLetter[3]));
					
					break;
				case 5:
					// 5개가 조합됬었음(초성+겹중성+겹종성) -> 초성+겹중성+종성
					nHangulCode[3] = findHangulByStr(2, ppPrevLetter[3]);  // 이후 겹종성 조합을 위해서 초기화
					
					// 겹종성 조합 가능 여부 확인
					tempStr = ppPrevLetter[1];
					tempStr.append(ppPrevLetter[2]);
					wcTempHangul = 0xAC00 + (findHangulByStr(0, ppPrevLetter[0]) * 21 * 28) + (findHangulByStr(1, tempStr.c_str()) * 28) + findHangulByStr(2, ppPrevLetter[3]);
					break;
				}

				strRet.push_back(wcTempHangul);

				for (int i = 4; i > 0; i--)
					ppPrevLetter[i][0] = ppPrevLetter[i - 1][0];

				usedAlphabet--;
			}
			else
			{
				// 임시 변수를 초기화 하고 커서를 이동
				clearTempHangul();
				moveCursorTo(nCursorPos - 1);
			}
		}
		
		return strRet + testad;
	}

	// 키보드 영문을 키보드 한글 문자로 대치한다.
	wchar_t wcCurrentHangle = HANGUL_KEYMAP[c];
	if (wcCurrentHangle == 0x0000)
	{
		// 키보드 영문을 키보드 한글 문자로 대치하지 못했을 경우 -> 입력한걸 그대로 출력한다.
		clearTempHangul();
		
		// 커서는 항상 마지막 위치에 있도록 한다.(한글이 아니라면 커서는 밀린다)
		strRet.push_back(c);
		strRet.append(testad);
		
		// 커서가 맨 마지막에 있지 않았으면
		if (nCursorPos != strCurrentContext.length())
			moveCursorTo(nCursorPos + 1); // 커서 위치를 기억하고 있다가 다음에도 계속 커서 이후에 글씨를 넣어줘야 함
		else
			moveCursorTo(static_cast<int>(strRet.length()));
		
		return strRet;
	}

	// 검사 결과 저장용 변수 선언
	int nLetter = 0;
	
	// 2번째 파라미터 <char c>는 뒤가 '\0'으로 종결됨이 보장되지 않기에 따로 처리함.
	charToStr(cTempLetter, c);
	const char* cc = (const char*)cTempLetter;
	
	if (nHangulCode[0] == -1 || usedAlphabet == 5)
	{
		// 입력된 문자가 초성인지 검사한다.
		nLetter = findHangulByStr(0, cc);

		if (nLetter != -1)
		{
			clearTempHangul();
			nHangulCode[0] = nLetter * 21 * 28;
		}
		else
		{
			// 이전 입력 문자가 모음이면 처리할 필요가 없음
			// 현재 입력 문자가 모음이여야 함
			int nPrev = findHangulByStr(1, (const char*)ppPrevLetter[4]);
			nLetter = findHangulByStr(1, cc);
			if (nPrev == -1 && nLetter != -1)
			{
				// 겹받침 + 모음
				int nPPrev = findHangulByStr(2, (const char*)ppPrevLetter[3]);
				if (nPPrev != -1)
				{
					// 겹받침을 만들어내서 코드를 찾은 뒤, 빼준다.
					std::string str(ppPrevLetter[3]);
					str.push_back(ppPrevLetter[4][0]);
					wchar_t wcPrev = strGetLastChar(strRet) - findHangulByStr(2, str.c_str());
					wcPrev += nPPrev;  // 홑받침으로 만든다.
					strBackspace(strRet);
					strRet.push_back(wcPrev);

					// 이전 입력 문자와 입력된 모음을 결합한다.
					clearTempHangul();
					usedAlphabet++;
					nPrev = findHangulByStr(0, (const char*)ppPrevLetter[4]);
					nHangulCode[0] = nPrev * 21 * 28;
					nHangulCode[1] = nLetter * 28;
					wcCurrentHangle = 0xAC00 + nHangulCode[0] + nHangulCode[1];
				}
			}
			else
				clearTempHangul();
		}
	}
	else if (nHangulCode[1] == -1 && nHangulCode[2] == -1)
	{
		// 초성 + (중성 or 초성)
		// 입력된 문자가 중성인지 검사한다.
		nLetter = findHangulByStr(1, cc);
		if (nLetter != -1)
		{
			nHangulCode[1] = nLetter * 28;
			wcCurrentHangle = 0xAC00 + nHangulCode[0] + nHangulCode[1];
			strBackspace(strRet);
			moveCursorTo(nCursorPos - 1);
		}
		else
		{
			// 입력된 문자가 중성이 아니라면 초성이라고 친다.
			nLetter = findHangulByStr(0, cc);
			if (nLetter != -1)
			{
				clearTempHangul();
				nHangulCode[0] = nLetter * 21 * 28;
			}
		}
	}
	else if (nHangulCode[1] != -1 && nHangulCode[2] == -1)
	{
		// 중성 + (겹중성 or 종성)
		// 입력된 문자가 2번째로 올 수 있는 중성인지 검사한다.
		std::string str(HANGUL_JUNG[nHangulCode[1] / 28]);
		str.push_back(c);
		nLetter = findHangulByStr(1, str.c_str());
		if (nLetter != -1)
		{
			nHangulCode[1] = nLetter * 28;
			nHangulCode[2] = -2;
			wcCurrentHangle = 0xAC00 + nHangulCode[0] + nHangulCode[1];
			strBackspace(strRet);
			moveCursorTo(nCursorPos - 1);
		}
		else
		{
			// 입력된 문자가 겹중성이 될 수 없다면 - 모음을 쳤거나, 종성을 친 경우다. + 종성이 될 수 없는 쌍자음을 친 경우(ㅃ, ㅉ, ㄸ)
			
			// 종성이 될 수 없는 쌍자음인지 검사한다.
			if (c == 'Q' || c == 'W' || c == 'E')
			{
				clearTempHangul();
				// 초성으로 넣어준다.
				nLetter = findHangulByStr(0, cc);
				if (nLetter != -1)
				{
					nHangulCode[0] = nLetter * 21 * 28;
				}
			}
			else
			{
				// 종성인지 검사한다.
				nLetter = findHangulByStr(2, cc);
				if (nLetter != -1)
				{
					nHangulCode[2] -= 1;
					nHangulCode[3] = nLetter;
					wcCurrentHangle = 0xAC00 + nHangulCode[0] + nHangulCode[1] + nHangulCode[3];  // 초 + 중 + 종
					strBackspace(strRet);
					moveCursorTo(nCursorPos-1);
				}
				else
					clearTempHangul();
			}
		}
	}
	else if (nHangulCode[2] != -1 && nHangulCode[3] == -1)
	{
		// 겹중성 + (종성 or 모음, 초성)
		// 입력된 문자가 종성인지 검사한다.
		nLetter = findHangulByStr(2, cc);
		if (nLetter != -1)
		{
			nHangulCode[3] = nLetter;
			wcCurrentHangle = 0xAC00 + nHangulCode[0] + nHangulCode[1] + nHangulCode[3];  // 초 + 중 + 종
			strBackspace(strRet);
			moveCursorTo(nCursorPos - 1);
		}
		else
		{
			// 입력된 문자가 종성이 아니라면 겹중성이 될 수 없는 모음이 왔거나 초성이 온 경우다.
			// 초성인지 검사한다.
			nLetter = findHangulByStr(0, cc);
			if (nLetter != -1)
			{
				clearTempHangul();
				nHangulCode[0] = nLetter * 21 * 28;
			}
			else
				clearTempHangul();
		}
	}
	else if (nHangulCode[3] != -1)
	{
		// 종성 + (겹종성 or 모음, 초성)
		// 입력된 문자가 2번째로 올 수 있는 종성인지 검사한다.
		std::string str(HANGUL_JONG[nHangulCode[3]]);
		str.push_back(c);
		nLetter = findHangulByStr(2, str.c_str());
		if (nLetter != -1)
		{
			nHangulCode[3] = nLetter;
			wcCurrentHangle = 0xAC00 + nHangulCode[0] + nHangulCode[1] + nHangulCode[3];  // 초 + 중 + 2번째 종성
			strBackspace(strRet);
			moveCursorTo(nCursorPos - 1);
		}
		else
		{
			// 입력된 문자가 겹종성이 될 수 없다면 - 모음을 쳤거나, 초성을 친 경우다.
			// 중성(모음)인지 검사한다.
			nLetter = findHangulByStr(1, cc);
			if (nLetter != -1)
			{
				// 앞 글씨의 받침과 입력된 중성을 결합한다.

				// 이전 입력 문자(종성)을 제거한다.
				wchar_t wcPrev = strGetLastChar(strRet);
				int nPrev = findHangulByStr(2, (const char*)ppPrevLetter[4]);
				wcPrev -= nPrev;
				strBackspace(strRet);
				strRet.push_back(wcPrev);

				// 이전 입력 문자와 입력된 모음을 결합한다.
				clearTempHangul();
				usedAlphabet++;
				nPrev = findHangulByStr(0, (const char*)ppPrevLetter[4]);
				nHangulCode[0] = nPrev * 21 * 28;
				nHangulCode[1] = nLetter * 28;
				wcCurrentHangle = 0xAC00 + nHangulCode[0] + nHangulCode[1];
			}
			else
			{
				// 중성이 아니라면 초성이라고 친다.
				nLetter = findHangulByStr(0, cc);
				if (nLetter != -1)
				{
					clearTempHangul();
					nHangulCode[0] = nLetter * 21 * 28;
				}
			}
		}
	}

	// 이전 글씨를 저장한다.
	// 용도: 종성과 모음의 조합
	// 인덱스 0 - 제일 오래된 문자, 인덱스 4 - 최신 입력 문자
	for (int i = 0; i < 4; i++)
		charToStr(ppPrevLetter[i], ppPrevLetter[i+1][0]);
	charToStr(ppPrevLetter[4], c);
	
	strRet.push_back(wcCurrentHangle);
	strRet.append(testad);
	// 커서가 맨 마지막에 있지 않았으면
	if (nCursorPos != strCurrentContext.length())
		moveCursorTo(nCursorPos + 1); // 커서 위치를 기억하고 있다가 다음에도 계속 커서 이후에 글씨를 넣어줘야 함
	else
		moveCursorTo(static_cast<int>(strRet.length())); // 커서 위치를 그냥 맨 마지막으로 옮긴다.
	
	// 에디팅 상태는 한글이 입력되었을 때 항상 true여야 한다.
	isEditing = true;
	
	// 한 글자에 사용된 획 수를 저장한다.
	// 에디터에서 한 획씩 지울 때 사용
	usedAlphabet++;
	
	return strRet;
}