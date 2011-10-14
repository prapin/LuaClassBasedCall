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
	OutputStrings();
	WideScript();
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

bool TestUnicode::OutputStrings()
{
	const wchar_t*  v1;
	wchar_t v2[10];
	size_t v2len = countof(v2);
	const wchar_t*  v3;
	return OutputCommonStart("OutputStrings", "return 'Hello ', 'World ', '\\194\\128\\223\\191\\224\\160\\128\\239\\191\\191\\237\\159\\191\\238\\128\\128\\239\\191\\189'", 
		Outputs(v1,Output(v2len, v2), v3)) &&
		OutputCommonEnd(0x50f4607d, "{%S,%d:%S,{%X,%X,%X,%X,%X,%X,%X}}", v1,v2len,v2,v3[0],v3[1],v3[2],v3[3],v3[4],v3[5],v3[6]);
}

bool TestUnicode::WideScript()
{
	lua::Lua<wchar_t> L(Lua);
	PSTRING testname = "WideScript";
	const wchar_t script[] = { 'r','e','t','u','r','n',39, 0x7F, 0x80, 0x7FF, 0x800, 0xD7FF, 0xE000, 39, 0 }; 
	const wchar_t*  v3;
	const wchar_t* error = L.PCall(script,  Outputs(v3));
	char buffer[100];
	if(error)
	{
		sprintf(buffer, "%S", script);
		return Report(testname, buffer);
	}
	sprintf(buffer, "{%X,%X,%X,%X,%X,%X}", v3[0],v3[1],v3[2],v3[3],v3[4],v3[5]);
	return CrcAndReport(testname, 0x5C949D34, buffer);
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
