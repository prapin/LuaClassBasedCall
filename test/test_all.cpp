// Two configuration flags
#define LCBC_USE_MFC 0
#define LCBC_USE_CSL 1

#define _CRT_SECURE_NO_WARNINGS
#include <assert.h>
#include <tchar.h>
#include "lgencall.hpp"
#include "test.hpp"


using namespace lua;
using namespace std;

#define countof(a)  (sizeof(a) / sizeof(a[0]))

Test::Test(bool fverbose)
:	fVerbose(fverbose),
	PassedCnt(0),
	FailedCnt(0)
{
	MakeCrcTable();
}
		
bool Test::Report(bool result, PSTRING error_msg, PSTRING test_msg)
{
	if(result)
	{
		PassedCnt++;
		sprintf("[PASSED] %s\n", fVerbose ? test_msg : "");
	}
	else
	{
		FailedCnt++;
		sprintf("[FAILED] %s %s\n", error_msg, fVerbose ? test_msg : "");
	}
	return result;
}

void Test::MakeCrcTable()
{
    // generate a crc for every 8-bit value
    for (int n = 0; n < 256; n++) {
        uint32_t c = (uint32_t)n;
        for (int k = 0; k < 8; k++)
            c = c & 1 ? 0xedb88320UL ^ (c >> 1) : c >> 1;
        CrcTable[n] = c;
    }
 }

uint32_t Test::ComputeCrc(const uint8_t* buf, size_t len)
{
    uint32_t crc = 0xffffffffUL;
    for(size_t i=0;i<len;i++)
        crc = CrcTable[((int)crc ^ (buf[i])) & 0xff] ^ (crc >> 8);
    return crc ^ 0xffffffffUL;
}


bool Test::InputCommon(uint32_t crc_ref, const Inputs& inputs)
{
	const char* error;
	const char* dump;
	error = Lua.PCall(_T("return DataDumper{...}"), inputs, Outputs(dump));
	if(error)
		return Report(error, "");
	uint32_t crc = ComputeCrc((const uint8_t*)dump, strlen(dump));
	lua_pushfstring(Lua, "%08X <=> %08X %s", crc, crc_ref, dump);
	return Report(crc == crc_ref, "CRC missmatch", lua_tostring(Lua, -1));
}

bool Test::InputNumbers()
{
	return InputCommon(0xA46633F1, Inputs(-4, (int)0xFFFFFFFF, 0xFFFFFFFF, 3.1415926535f, 3.1415926535));
}

static void test_in_other_scalars(Lua& L)
{
	L.PCall(_T("for k,v in pairs{...} do print(k, type(v), v) end"), 
		Inputs(false, true, nil, _T("Hello"), (const lua_State*)L, (const void*)L));
}

static int cFunction(lua_State* L)
{
	printf("%s\n", luaL_checkstring(L, 1));
	return 0;
}

struct tMessage
{
	int Number;
	const char* Text;
};

namespace lua {
template<> void Input::PushValue<tMessage>(lua_State* L) const
{
	tMessage* msg = (tMessage*) PointerValue;
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, msg->Number);
	lua_setfield(L, -2, "Number");
	lua_pushstring(L, msg->Text);
	lua_setfield(L, -2, "Text");
}
}

static void test_in_function_custom(Lua& L)
{
	tMessage msg = { 10, "Hello world" };
	L.UCall(_T("local fct, msg = ...; fct(msg.Number..': '..msg.Text)"), 
		Inputs(cFunction, (const tMessage*)&msg));
}

static void test_in_arrays(Lua& L)
{
	short array[] = { 1,2,3 };
	L.UCall(_T("for k,v in pairs{...} do print(k, #v, table.concat(v, ', ')) end"), 
		Inputs(Input(2, array), Input(5, "Hello"), Input(countof(array), array)));
}

static void test_in_strings(Lua& L)
{
	unsigned char data[] = { 200, 100, 0, 3, 5, 0 };
	string s1("P2\0P3", 5);
	wstring s2(L"P4\0P5", 5);
	//CStringA s3("P6\0P7", 5);
	//CStringW s4("P8\0P9", 5);
	L.UCall(_T("for k,v in pairs{...} do print(k, v:gsub('.', ")
		_T("function(c) return '\\\\'..c:byte() end)) end"), 
		Inputs("Hello", Input(_T("P1\0P2"), 6), Input((const char*)data, sizeof(data)), 
		L"ete", s1, s2, s1, s2));
}
#if LCBC_USE_CSL
static void test_in_string_lists(Lua& L)
{
	const char* a1[] = {"s1", "s2", "s3" };
	const wchar_t* const a2[] = {L"s4", L"s5", L"s6" };
	vector<const char*> v1; v1.push_back("s7"); v1.push_back("s8"); 
	vector<const wchar_t*> v2; v2.push_back(L"s9"); v2.push_back(L"s10"); 
	L.UCall(_T("for k,v in pairs{...} do print(k, #v, table.concat(v, ',')) end"),
		Inputs(Input(3, a1), Input(3, a2), v1, v2));
}

static void test_out_numbers(Lua& L)
{
	char var1; unsigned short var2; int var3;
	float var4; double var5;
	L.PCall(_T("return 1, 2, 3, 4, 5"),
		Outputs(var1, var2, var3, var4, var5));
	printf("%d %u %d %f %f\n", var1, var2, var3, var4, var5);
}

static void test_out_other_scalars(Lua& L)
{
	bool bool1; bool bool2; 
	const char* str1; const void* ptr;
	char str2[10];
	size_t strsize = countof(str2);
	L.UCall(_T("return true, false, 'dummy', 'Hello', io.stdin"), 
		Outputs(bool1, bool2, str1, Output(strsize, str2), 
		Output(ptr, strsize)));
	printf("%hd %hd %s %s %p\n", bool1, bool2, str1, str2, ptr);
}

namespace lua {
template<> void Output::GetValue<tMessage>(lua_State* L, int idx) const
{
	tMessage* msg = (tMessage*) PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	lua_getfield(L, idx, "Number");
	msg->Number = luaL_checkint(L, -1);
	lua_getfield(L, idx, "Text");
	msg->Text = luaL_checkstring(L, -1);
	lua_pop(L, 2);
}}

static void test_out_function_custom(Lua& L)
{
	lua_CFunction fct;
	tMessage msg;
	L.UCall(_T("return print, {Number=3, Text='Hello World!'}"), 
		Outputs(fct, msg));
	printf("Function: %p, message: %d, %s\n", fct, msg.Number, msg.Text);
}

static void test_out_arrays(Lua& L)
{
	unsigned int int_a[3];
	bool bool_a[4];
	vector<char> str; 
	vector<short> short_a;
	size_t int_len = countof(int_a);
	size_t bool_len = countof(bool_a);
	L.PCall(_T("return {1,2,3,4},{72,101,108,108,111,0}, {5,6,7}, {false,true}"), 
		Outputs(Output(int_len, int_a), str, short_a, Output(bool_len, bool_a)));
	printf("int_a = {%u,%u,%u}\nstr = %c%c%c%c%c%c\npshort[%d]=%d\nbool_a = #%d:{%d,%d,%d,%d}\n", 
		int_a[0], int_a[1], int_a[2], str[0], str[1], str[2], str[3], str[4], str[5], short_a.size()-1, short_a[short_a.size()-1],
		bool_len, bool_a[0], bool_a[1], bool_a[2], bool_a[3]);
	/*CByteArray ba;
	CArray<int, int> a;
	Inputs test(a);
	Outputs test2(a);*/
}

static void test_out_strings(Lua& L)
{
	const TCHAR *str1;
	TCHAR str2[20];
	size_t str2_len = countof(str2);
	char str3[10];
	size_t str3_len = countof(str3);
	unsigned char data[6];
	size_t len = sizeof(data);
	const wchar_t* wstr;
	L.UCall(_T("return 'Hello', ' Wor', 'ld!', '\\0\\5\\200\\0', 'Unicode'"),
		Outputs(str1, Output(str2_len, str2), Output(str3_len, str3), Output(len, (char*)data), wstr));
	_tprintf(_T("%s%s"), str1, str2);
	printf("%s\ndata (%d bytes): %02X %02X %02X %02X %02X\n", 
		str3, len, data[0],data[1],data[2],data[3],data[4]);
	printf("wstr = %S\n", wstr);
	string s1;
	wstring s2;
	//CStringA s3;
	//CStringW s4;
	L.UCall("a='Hello\\0World\\0' return a,a", Outputs(s1, s2));
	printf("%d %d\n", s1.size(), s2.size());
}

static void test_out_string_lists(Lua& L)
{
	vector<const char*> str1;
	vector<const wchar_t*> str2;
	vector<string> str3;
	vector<wstring> str4;
	const char* str5[2];
	size_t str5len = countof(str5);
	const wchar_t* str6[3];
	size_t str6len = countof(str6);
	L.UCall(_T("return {1,2,3},{44,55,66},{10,9,8,7},{11,12},{13,14,15},{16,17}"),
		Outputs(str1, str2, str3, str4, Output(str5len, str5), Output(str6len, str6)));
	printf("str1 = #%d:{%s, %s, %s}\n", str1.size(), str1[0], str1[1], str1[2]);
	printf("str2 = #%d:{%S, %S, %S}\n", str2.size(), str2[0], str2[1], str2[2]);
	printf("str3 = #%d:{%s, %s, %s, %s}\n", str3.size(), str3[0].c_str(), str3[1].c_str(), str3[2].c_str(), str3[3].c_str());
	printf("str4 = #%d:{%S, %S}\n", str4.size(), str4[0].c_str(), str4[1].c_str());
	printf("str5 = #%d:{%s, %s}\n", str5len, str5[0], str5[1]);
	printf("str6 = #%d:{%S, %S, %S}\n", str6len, str6[0], str6[1], str6[2]);
}
static void test_script_errors(Lua& L)
{
	_tprintf(_T("%s\n"), L.PCall(_T("a = 'toto' + 2")));
	try
	{
		L.ECall(_T("a = 'toto' + 2"));
	}
	catch(Error e)
	{
		_tprintf(_T("%s\n"), e.str());
	}
}

#define IDENTITY _T("print(DataDumper{...}) return ...")
static void test_ident_booleans(Lua& L)
{
	size_t i;
	bool arr1[10], arr2[10];
	vector<bool>v1, v2;
	size_t arr2len = countof(arr2);
	for(i=0;i<10;i++)
		arr1[i] = rand() > 16000;
	L.UCall(IDENTITY, Input(10, arr1), Output(arr2len, arr2));
	assert(memcmp(arr1, arr2, sizeof(arr1)) == 0);
	for(i=0;i<10;i++)
		v1.push_back(rand() > 16000);
	L.UCall(IDENTITY, Inputs(v1), Outputs(v2));
	assert(v1.size() == v2.size());
	for(i=0;i<10;i++)
		assert(v1[i] == v2[i]);
}

static void test_ident_doubles(Lua& L)
{
	vector<double> d1, d2;
	d1.push_back(2.45); d1.push_back(5.3e4); d1.push_back(1000);
	L.UCall(IDENTITY, Input(d1), Output(d2));
	assert(d1.size() == d2.size());
	for(size_t i=0;i<d1.size();i++)
		assert(d1[i] == d2[i]);
}

static void test_ident_strings(Lua& L)
{
	TCHAR strbuf1[20];
	const TCHAR* str1 = _T("Hello");
	const TCHAR* strptr1;
	size_t buf1size = countof(strbuf1);
	L.UCall(IDENTITY, str1, Output(buf1size, strbuf1));
	assert(_tcscmp(str1, strbuf1) == 0);
	L.UCall(IDENTITY, str1, Output(strptr1));
	assert(_tcscmp(str1, strptr1) == 0);
}

int main(int /*argc*/, char* /*argv*/[])
{
	Test test(true);
	test.InputNumbers();
	
#if 0	
	/*std::vector<std::string> vs;
	vs.push_back("Hello");
	vs.push_back("World");
	std::map<const char*, int> m;
	m["Hello"] = 2;*/
	Lua L;
	L.PCall("@dumper.lua");
	
	test_in_numbers(L);
	return 0;
	test_in_other_scalars(L);
	test_in_function_custom(L);
	test_in_arrays(L);
	test_in_strings(L);
	test_in_string_lists(L);

	test_out_numbers(L);
	test_out_other_scalars(L);
	test_out_function_custom(L);
	test_out_arrays(L);
	test_out_strings(L);
	test_out_string_lists(L);

	test_script_errors(L);

	test_ident_booleans(L);
	test_ident_doubles(L);
	test_ident_strings(L);
#endif
	return 0;
}

#endif
