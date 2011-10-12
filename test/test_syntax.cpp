// Configuration flags: using wide strings and exceptions
#define LCBC_USE_MFC 0
#define LCBC_USE_CSL 0
#define LCBC_USE_WIDESTRING 2
#define LCBC_USE_EXCEPTIONS 1

#define _CRT_SECURE_NO_WARNINGS
#include "lgencall.hpp"
#include "test.hpp"
#include "test_syntax.hpp"

using namespace lua;

bool TestSyntax::All()
{
	UCall();
	PCall();
	ECall();
	TCall();
	PShift();
	EShift();
	return FailedCnt == 0;
}

bool TestSyntax::UCall()
{
	int v1, v2, v3;
	const char* v4, *v5;
	char buffer[1000];
	Lua.UCall("return ...", 1, v1);
	Lua.UCall(L"return 2,3", Outputs(v2,v3));
	Lua.UCall("a={...}", Inputs(4,5));
	Lua.UCall("a[#a+1]=6");
	Lua.UCall("a[#a+1]=...", 7);
	Lua.UCall(L"a=DataDumper(a);return a", Output(v4));
	Lua.UCall(L"return DataDumper{...}", Inputs(8,9), Outputs(v5));

	sprintf(buffer, "%d,%d,%d,%s,%s", v1,v2,v3,v4,v5);
	return CrcAndReport("UCall", 0x8B914470, buffer);
}

bool TestSyntax::PCall()
{
	int v1, v2, v3;
	const char* v4, *v5;
	char buffer[1000];
	Lua.PCall(L"return ...", 1, v1);
	Lua.PCall("return 2,3", Outputs(v2,v3));
	Lua.PCall(L"a={...}", Inputs(4,5));
	Lua.PCall(L"a[#a+1]=6");
	Lua.PCall(L"a[#a+1]=...", 7);
	Lua.PCall("a=DataDumper(a);return a", Output(v4));
	Lua.PCall("return DataDumper{...}", Inputs(8,9), Outputs(v5));

	sprintf(buffer, "%d,%d,%d,%s,%s", v1,v2,v3,v4,v5);
	return CrcAndReport("PCall", 0x8B914470, buffer);
}

bool TestSyntax::ECall()
{
	int v1, v2, v3;
	const char* v4, *v5;
	char buffer[1000];
	Lua.ECall("return ...", 1, v1);
	Lua.ECall(L"return 2,3", Outputs(v2,v3));
	Lua.ECall("a={...}", Inputs(4,5));
	Lua.ECall("a[#a+1]=6");
	Lua.ECall("a[#a+1]=...", 7);
	Lua.ECall(L"a=DataDumper(a);return a", Output(v4));
	Lua.ECall(L"return DataDumper{...}", Inputs(8,9), Outputs(v5));

	sprintf(buffer, "%d,%d,%d,%s,%s", v1,v2,v3,v4,v5);
	return CrcAndReport("ECall", 0x8B914470, buffer);
}

bool TestSyntax::TCall()
{
	int v1, v2, v3;
	const char* v4, *v5;
	char buffer[1000];
	v1 = Lua.TCall<int>("return ...", 1);
	v2 = Lua.TCall<int>(L"return 2");
	v3 = Lua.TCall<int>("return 3");
	Lua.TCall<void>("a={...}", 4, 5);
	Lua.TCall<void>("a[#a+1]=6");
	Lua.TCall<void>("a[#a+1]=...", 7);
	v4 = Lua.TCall<const char*>(L"a=DataDumper(a);return a");
	v5 = Lua.TCall<const char*>(L"return DataDumper{...}", 8, 9);

	sprintf(buffer, "%d,%d,%d,%s,%s", v1,v2,v3,v4,v5);
	return CrcAndReport("TCall", 0x8B914470, buffer);
}

bool TestSyntax::PShift()
{
	int v1, v2, v3;
	const char* v4, *v5;
	char buffer[1000];
	Lua << 1 >> v1 | "return ...";
	Lua >> v2 >> v3 | L"return 2,3";
	Lua << 4 << 5 | "a={...}";
	Lua | "a[#a+1]=6";
	Lua << 7 | "a[#a+1]=...";
	Lua >> v4 | L"a=DataDumper(a);return a";
	Lua << 8 << 9 >> v5 | L"return DataDumper{...}";

	sprintf(buffer, "%d,%d,%d,%s,%s", v1,v2,v3,v4,v5);
	return CrcAndReport("PShift", 0x8B914470, buffer);
}

bool TestSyntax::EShift()
{
	int v1, v2, v3;
	const char* v4, *v5;
	char buffer[1000];
	Lua << 1 >> v1 & L"return ...";
	Lua >> v2 >> v3 & "return 2,3";
	Lua << 4 << 5 & L"a={...}";
	Lua & L"a[#a+1]=6";
	Lua << 7 & L"a[#a+1]=...";
	Lua >> v4 & "a=DataDumper(a);return a";
	Lua << 8 << 9 >> v5 & "return DataDumper{...}";

	sprintf(buffer, "%d,%d,%d,%s,%s", v1,v2,v3,v4,v5);
	return CrcAndReport("EShift", 0x8B914470, buffer);
}

int main(int argc, const PSTRING argv[])
{
	TestSyntax test(argc, argv);
	return test.All() ? 0 : 1;
}
