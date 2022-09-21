#include "stdafx.h"
#include "Hangul.h"

Hangul::Hangul()
{
	// ���� �ѱ��� ������ ������ ���� �������� �Ҵ�
	ppPrevLetter = new char* [5];
	for (int i = 0; i < 5; i++)
		ppPrevLetter[i] = new char[5]();
	cTempLetter = new char[2];
}

Hangul::~Hangul()
{
	// ���� �������� �Ҵ��� ������ ����
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
	// �ִ� ���̴� Ŀ���� ���������� �̵��� �� �־���� ��(���ڿ� ���̺��� Ŀ���� �ʵ���)
	
	if (index < 0)
		return;
	
	if (isUserInteract)
		clearTempHangul();
	
	if (unsigned(index) > maxLength)
		return;
	
	nCursorPos = index;

	std::wstring aaaa = L"���� Ŀ�� ��ġ: ";
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
	
	// Ŀ�� ���ķ� ���ڸ� ������.
	std::wstring strRet = strCurrentContext.substr(0, nCursorPos);
	std::wstring testad = strCurrentContext.substr(nCursorPos);

	if ('\b' == c)
	{
		if (strRet.length() >= 1)
		{
			strBackspace(strRet);
			
			if (isEditing && usedAlphabet > 1)
			{
				// ������ ���¶�� �ڸ����� �ϳ��� �����ִ� �κ�
				wchar_t wcTempHangul = 0;
				std::string tempStr;
				int tempIndex = 0;
				
				switch (usedAlphabet)
				{
				case 2:
					// 2���� ���յǾ�����(�ʼ�+�߼�) -> �ʼ��� ����� ��
					nHangulCode[1] = -1;
					wcTempHangul = HANGUL_KEYMAP[ppPrevLetter[3][0]];
					break;
				case 3:
					// 3���� ���յǾ�����(�ʼ�+���߼�, �ʼ�+�߼�+����) -> �ʼ�+�߼��� ����� ��
					nHangulCode[0] = findHangulByStr(0, ppPrevLetter[2]) * 21 * 28;
					nHangulCode[1] = findHangulByStr(1, ppPrevLetter[3]) * 28;  // ���� ������ ������ ���ؼ� �ʱ�ȭ
					nHangulCode[2] = -1;
					wcTempHangul = 0xAC00 + nHangulCode[0] + nHangulCode[1];
					
					break;
				case 4:
					// 4���� ���Չ����(�ʼ�+���߼�+����, �ʼ�+�߼�+������) -> �ʼ�+���߼�, �ʼ�+�߼�+����
					nHangulCode[3] = -1;
					
					// ���߼� ���� ���� ���� Ȯ��
					tempStr = ppPrevLetter[2];
					tempStr.append(ppPrevLetter[3]);
					tempIndex = findHangulByStr(1, tempStr.c_str());
					
					if (tempIndex != -1)
						wcTempHangul = 0xAC00 + (findHangulByStr(0, ppPrevLetter[1]) * 21 * 28) + (tempIndex * 28);  // ���߼�	
					else
						wcTempHangul = 0xAC00 + (findHangulByStr(0, ppPrevLetter[1]) * 21 * 28) + (findHangulByStr(1, ppPrevLetter[2]) * 28) + (findHangulByStr(2, ppPrevLetter[3]));
					
					break;
				case 5:
					// 5���� ���Չ����(�ʼ�+���߼�+������) -> �ʼ�+���߼�+����
					nHangulCode[3] = findHangulByStr(2, ppPrevLetter[3]);  // ���� ������ ������ ���ؼ� �ʱ�ȭ
					
					// ������ ���� ���� ���� Ȯ��
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
				// �ӽ� ������ �ʱ�ȭ �ϰ� Ŀ���� �̵�
				clearTempHangul();
				moveCursorTo(nCursorPos - 1);
			}
		}
		
		return strRet + testad;
	}

	// Ű���� ������ Ű���� �ѱ� ���ڷ� ��ġ�Ѵ�.
	wchar_t wcCurrentHangle = HANGUL_KEYMAP[c];
	if (wcCurrentHangle == 0x0000)
	{
		// Ű���� ������ Ű���� �ѱ� ���ڷ� ��ġ���� ������ ��� -> �Է��Ѱ� �״�� ����Ѵ�.
		clearTempHangul();
		
		// Ŀ���� �׻� ������ ��ġ�� �ֵ��� �Ѵ�.(�ѱ��� �ƴ϶�� Ŀ���� �и���)
		strRet.push_back(c);
		strRet.append(testad);
		
		// Ŀ���� �� �������� ���� �ʾ�����
		if (nCursorPos != strCurrentContext.length())
			moveCursorTo(nCursorPos + 1); // Ŀ�� ��ġ�� ����ϰ� �ִٰ� �������� ��� Ŀ�� ���Ŀ� �۾��� �־���� ��
		else
			moveCursorTo(static_cast<int>(strRet.length()));
		
		return strRet;
	}

	// �˻� ��� ����� ���� ����
	int nLetter = 0;
	
	// 2��° �Ķ���� <char c>�� �ڰ� '\0'���� ������� ������� �ʱ⿡ ���� ó����.
	charToStr(cTempLetter, c);
	const char* cc = (const char*)cTempLetter;
	
	if (nHangulCode[0] == -1 || usedAlphabet == 5)
	{
		// �Էµ� ���ڰ� �ʼ����� �˻��Ѵ�.
		nLetter = findHangulByStr(0, cc);

		if (nLetter != -1)
		{
			clearTempHangul();
			nHangulCode[0] = nLetter * 21 * 28;
		}
		else
		{
			// ���� �Է� ���ڰ� �����̸� ó���� �ʿ䰡 ����
			// ���� �Է� ���ڰ� �����̿��� ��
			int nPrev = findHangulByStr(1, (const char*)ppPrevLetter[4]);
			nLetter = findHangulByStr(1, cc);
			if (nPrev == -1 && nLetter != -1)
			{
				// ���ħ + ����
				int nPPrev = findHangulByStr(2, (const char*)ppPrevLetter[3]);
				if (nPPrev != -1)
				{
					// ���ħ�� ������ �ڵ带 ã�� ��, ���ش�.
					std::string str(ppPrevLetter[3]);
					str.push_back(ppPrevLetter[4][0]);
					wchar_t wcPrev = strGetLastChar(strRet) - findHangulByStr(2, str.c_str());
					wcPrev += nPPrev;  // Ȭ��ħ���� �����.
					strBackspace(strRet);
					strRet.push_back(wcPrev);

					// ���� �Է� ���ڿ� �Էµ� ������ �����Ѵ�.
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
		// �ʼ� + (�߼� or �ʼ�)
		// �Էµ� ���ڰ� �߼����� �˻��Ѵ�.
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
			// �Էµ� ���ڰ� �߼��� �ƴ϶�� �ʼ��̶�� ģ��.
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
		// �߼� + (���߼� or ����)
		// �Էµ� ���ڰ� 2��°�� �� �� �ִ� �߼����� �˻��Ѵ�.
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
			// �Էµ� ���ڰ� ���߼��� �� �� ���ٸ� - ������ �ưų�, ������ ģ ����. + ������ �� �� ���� �������� ģ ���(��, ��, ��)
			
			// ������ �� �� ���� ���������� �˻��Ѵ�.
			if (c == 'Q' || c == 'W' || c == 'E')
			{
				clearTempHangul();
				// �ʼ����� �־��ش�.
				nLetter = findHangulByStr(0, cc);
				if (nLetter != -1)
				{
					nHangulCode[0] = nLetter * 21 * 28;
				}
			}
			else
			{
				// �������� �˻��Ѵ�.
				nLetter = findHangulByStr(2, cc);
				if (nLetter != -1)
				{
					nHangulCode[2] -= 1;
					nHangulCode[3] = nLetter;
					wcCurrentHangle = 0xAC00 + nHangulCode[0] + nHangulCode[1] + nHangulCode[3];  // �� + �� + ��
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
		// ���߼� + (���� or ����, �ʼ�)
		// �Էµ� ���ڰ� �������� �˻��Ѵ�.
		nLetter = findHangulByStr(2, cc);
		if (nLetter != -1)
		{
			nHangulCode[3] = nLetter;
			wcCurrentHangle = 0xAC00 + nHangulCode[0] + nHangulCode[1] + nHangulCode[3];  // �� + �� + ��
			strBackspace(strRet);
			moveCursorTo(nCursorPos - 1);
		}
		else
		{
			// �Էµ� ���ڰ� ������ �ƴ϶�� ���߼��� �� �� ���� ������ �԰ų� �ʼ��� �� ����.
			// �ʼ����� �˻��Ѵ�.
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
		// ���� + (������ or ����, �ʼ�)
		// �Էµ� ���ڰ� 2��°�� �� �� �ִ� �������� �˻��Ѵ�.
		std::string str(HANGUL_JONG[nHangulCode[3]]);
		str.push_back(c);
		nLetter = findHangulByStr(2, str.c_str());
		if (nLetter != -1)
		{
			nHangulCode[3] = nLetter;
			wcCurrentHangle = 0xAC00 + nHangulCode[0] + nHangulCode[1] + nHangulCode[3];  // �� + �� + 2��° ����
			strBackspace(strRet);
			moveCursorTo(nCursorPos - 1);
		}
		else
		{
			// �Էµ� ���ڰ� �������� �� �� ���ٸ� - ������ �ưų�, �ʼ��� ģ ����.
			// �߼�(����)���� �˻��Ѵ�.
			nLetter = findHangulByStr(1, cc);
			if (nLetter != -1)
			{
				// �� �۾��� ��ħ�� �Էµ� �߼��� �����Ѵ�.

				// ���� �Է� ����(����)�� �����Ѵ�.
				wchar_t wcPrev = strGetLastChar(strRet);
				int nPrev = findHangulByStr(2, (const char*)ppPrevLetter[4]);
				wcPrev -= nPrev;
				strBackspace(strRet);
				strRet.push_back(wcPrev);

				// ���� �Է� ���ڿ� �Էµ� ������ �����Ѵ�.
				clearTempHangul();
				usedAlphabet++;
				nPrev = findHangulByStr(0, (const char*)ppPrevLetter[4]);
				nHangulCode[0] = nPrev * 21 * 28;
				nHangulCode[1] = nLetter * 28;
				wcCurrentHangle = 0xAC00 + nHangulCode[0] + nHangulCode[1];
			}
			else
			{
				// �߼��� �ƴ϶�� �ʼ��̶�� ģ��.
				nLetter = findHangulByStr(0, cc);
				if (nLetter != -1)
				{
					clearTempHangul();
					nHangulCode[0] = nLetter * 21 * 28;
				}
			}
		}
	}

	// ���� �۾��� �����Ѵ�.
	// �뵵: ������ ������ ����
	// �ε��� 0 - ���� ������ ����, �ε��� 4 - �ֽ� �Է� ����
	for (int i = 0; i < 4; i++)
		charToStr(ppPrevLetter[i], ppPrevLetter[i+1][0]);
	charToStr(ppPrevLetter[4], c);
	
	strRet.push_back(wcCurrentHangle);
	strRet.append(testad);
	// Ŀ���� �� �������� ���� �ʾ�����
	if (nCursorPos != strCurrentContext.length())
		moveCursorTo(nCursorPos + 1); // Ŀ�� ��ġ�� ����ϰ� �ִٰ� �������� ��� Ŀ�� ���Ŀ� �۾��� �־���� ��
	else
		moveCursorTo(static_cast<int>(strRet.length())); // Ŀ�� ��ġ�� �׳� �� ���������� �ű��.
	
	// ������ ���´� �ѱ��� �ԷµǾ��� �� �׻� true���� �Ѵ�.
	isEditing = true;
	
	// �� ���ڿ� ���� ȹ ���� �����Ѵ�.
	// �����Ϳ��� �� ȹ�� ���� �� ���
	usedAlphabet++;
	
	return strRet;
}