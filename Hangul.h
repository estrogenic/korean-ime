#pragma once

// (유니코드 완성형 한글코드) = 0xAC00 + 28*21*(초성인덱스) + 28*(중성인덱스) + (종성인덱스)
// prefix + ㅁ + suffix -> strCurrentContext 로 하지 말고 커서 기준으로

class Hangul
	// HangulAssemble
	// 상위 클래스가 필요할 수 있음
{
public:
	Hangul();
	~Hangul();
	// TODO: 아스키 코드로 키 입력을 받아올 수 있게 하기
	// TODO: strCurrentContext를 입력받지 않게 하기
	// + 이 클래스의 기능을 이름으로 잘 드러내기(moveCursorTo는 필요하지 않음)
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