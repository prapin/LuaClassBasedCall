// Configuration flags: enables wide strings. 
// 
#define LCBC_USE_MFC 0
#define LCBC_USE_CSL 0
#define LCBC_USE_EXCEPTIONS 0
#ifndef LCBC_USE_WIDESTRING
#define LCBC_USE_WIDESTRING 3 // But try with 1 and 2 too.
#endif

#define _CRT_SECURE_NO_WARNINGS
#include "lgencall.hpp"
#include "test.hpp"
#include "test_unicode.hpp"

using namespace lua;

#define countof(a)  (sizeof(a) / sizeof(a[0]))

bool TestUnicode::All()
{
	InputStrings();
	OverLongUTF8();
	return FailedCnt == 0;
}

bool TestUnicode::InputStrings()
{
	const wchar_t* v1=L"Hello";
	const wchar_t* v2=L"World";
	wchar_t v3[] = { 0, 0x7F, 0x80, 0x7FF, 0x800, 0xFFFF, 0xD7FF, 0xE000, 0xFFFD };
	return InputCommon("InputNumbers", 0x148e1629, Inputs(v1, Input(v2, 5), Input(countof(v3), v3)));
}

bool TestUnicode::OverLongUTF8()
{
	static const char* overlongs[] = {
		// These are valid UTF-8 sequences
		"\x00","\x7F","\xC2\x80","\xDF\xBF","\xE0\xA0\x80",
		"\xEF\xBF\xBF","\xED\x9F\xBF","\xEE\x80\x80","\xEF\xBF\xBD",
		// Those are invalid (overlong) ones
		"\xc0\xaf",	"\xe0\x80\xaf",	"\xf0\x80\x80\xaf",
		"\xf8\x80\x80\x80\xaf",	"\xfc\x80\x80\x80\x80\xaf",
		"\xc1\xbf", "\xe0\x9f\xbf", "\xf0\x8f\xbf\xbf",
		"\xf8\x87\xbf\xbf\xbf", "\xfc\x83\xbf\xbf\xbf\xbf",
		"\xc0\x80", "\xe0\x80\x80", "\xf0\x80\x80\x80",
		"\xf8\x80\x80\x80\x80", "\xfc\x80\x80\x80\x80\x80",
	};
	char buffer[countof(overlongs)+2] = {0};
	for(size_t i=0;i<countof(overlongs); i++)
	{
		const wchar_t* dest;
		bool OK = Lua.PCall("return ...", Input(overlongs[i]), Output(dest)) == NULL;
		sprintf(buffer+i,"%d", OK);
	}
	return CrcAndReport("OverLongUTF8", 0xf142c82b, buffer);
}

int main(int argc, const PSTRING argv[])
{
	TestUnicode test(argc, argv);
	return test.All() ? 0 : 1;
}
