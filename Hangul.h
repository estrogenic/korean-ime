#pragma once

// (�����ڵ� �ϼ��� �ѱ��ڵ�) = 0xAC00 + 28*21*(�ʼ��ε���) + 28*(�߼��ε���) + (�����ε���)
// prefix + �� + suffix -> strCurrentContext �� ���� ���� Ŀ�� ��������

class Hangul
	// HangulAssemble
	// ���� Ŭ������ �ʿ��� �� ����
{
public:
	Hangul();
	~Hangul();
	// TODO: �ƽ�Ű �ڵ�� Ű �Է��� �޾ƿ� �� �ְ� �ϱ�
	// TODO: strCurrentContext�� �Է¹��� �ʰ� �ϱ�
	// + �� Ŭ������ ����� �̸����� �� �巯����(moveCursorTo�� �ʿ����� ����)
	std::wstring AssembleHangul(std::wstring strCurrentContext, char c);
	void moveCursorTo(int index, bool isUserInteract = false, size_t maxLength = 0xFFFFFFFF);
	int nCursorPos = 0;

private:
	const char* HANGUL_CHO[19] = { "r", "R", "s", "e", "E", "f", "a", "q", "Q", "t", "T", "d", "w", "W", "c", "z", "x", "v", "g" };
	const char* HANGUL_JUNG[21] = { "k", "o", "i", "O", "j", "p", "u", "P", "h", "hk", "ho", "hl", "y", "n", "nj", "np", "nl", "b", "m", "ml", "l" };
	const char* HANGUL_JONG[28] = { " ", "r", "R", "rt", "s", "sw", "sg", "e", "f", "fr", "fa", "fq", "ft", "fx", "fv", "fg", "a", "q", "qt", "t", "T", "d", "w", "c", "z", "x", "v", "g" };
	std::map<char, wchar_t> HANGUL_KEYMAP = {
		{ 'q', 0x3142 },
		{ 'w', 0x3148 },
		{ 'e', 0x3137 },
		{ 'r', 0x3131 },
		{ 't', 0x3145 },
		{ 'y', 0x315B },
		{ 'u', 0x3155 },
		{ 'i', 0x3151 },
		{ 'o', 0x3150 },
		{ 'p', 0x3154 },
		{ 'a', 0x3141 },
		{ 's', 0x3134 },
		{ 'd', 0x3147 },
		{ 'f', 0x3139 },
		{ 'g', 0x314E },
		{ 'h', 0x3157 },
		{ 'j', 0x3153 },
		{ 'k', 0x314F },
		{ 'l', 0x3163 },
		{ 'z', 0x314B },
		{ 'x', 0x314C },
		{ 'c', 0x314A },
		{ 'v', 0x314D },
		{ 'b', 0x3160 },
		{ 'n', 0x315C },
		{ 'm', 0x3161 },
		{ 'Q', 0x3143 },
		{ 'W', 0x3149 },
		{ 'E', 0x3138 },
		{ 'R', 0x3132 },
		{ 'T', 0x3146 },
		{ 'O', 0x3152 },
		{ 'P', 0x3156 },
	};
	
	char** ppPrevLetter, * cTempLetter;
	int nHangulCode[4] = { -1 };
	int usedAlphabet = 0;
	bool isEditing = false;
	
	inline void clearTempHangul();
	inline void strBackspace(std::wstring& s);
	inline wchar_t strGetLastChar(std::wstring& s);
	inline void charToStr(char* cc, char& c);
	int findHangulByStr(uint8_t flagHangul, const char* strLetter);
	template <typename T, size_t size> int arrayIndexOf(T const (&array)[size], T const c);
};