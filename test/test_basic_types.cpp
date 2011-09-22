// Two configuration flags
#define LCBC_USE_MFC 0
#define LCBC_USE_CSL 0
#define LCBC_USE_WIDESTRING 0
#define LCBC_USE_EXCEPTIONS 0

#define _CRT_SECURE_NO_WARNINGS
#include "lgencall.hpp"
#include "test.cpp"
#include "test_basic_types.hpp"

using namespace lua;

#define countof(a)  (sizeof(a) / sizeof(a[0]))

bool TestBasicTypes::All()
{
	InputNumbers();
	InputOtherScalars();
	InputCustom();
	InputArrays();
	OutputNumbers();
	OutputOtherScalars();
	OutputCustom();
	OutputArrays();
	return FailedCnt == 0;
}

bool TestBasicTypes::InputNumbers()
{
	const int val = -12345678;
	return InputCommon("InputNumbers", 0xC781FACC, Inputs((int8_t)val, (uint8_t)val, (int16_t)val, (uint16_t)val, 
		(int32_t)val, (uint32_t)val, (int64_t)val, (uint64_t)(-val)*1000, 12.125f, 12.125));
}

static int cFunction(lua_State*)
{
	return 0;
}

bool TestBasicTypes::InputOtherScalars()
{
	lua_State* thread = Lua;
	void* lightuserdata = NULL;
	return InputCommon("InputOtherScalars", 0x496F6770, Inputs(false, true, nil, 
		"Hello", "World", thread, lightuserdata, cFunction));
}


namespace lua {
template<> void Input::PushValue<TestBasicTypes::tMessage>(lua_State* L) const
{
	TestBasicTypes::tMessage* msg = (TestBasicTypes::tMessage*) PointerValue;
	lua_createtable(L, 0, 2);
	lua_pushinteger(L, msg->Number);
	lua_setfield(L, -2, "Number");
	lua_pushstring(L, msg->Text);
	lua_setfield(L, -2, "Text");
}
}

bool TestBasicTypes::InputCustom()
{
	tMessage msg = { 10, "Hello world" };
	return InputCommon("InputCustom", 0xBC1D94E2, Inputs((const tMessage*)&msg));
}

bool TestBasicTypes::InputArrays()
{
	static int16_t arr1D[4] = { 1,2,3,4 };
	static uint32_t arr2D[2][3] = {{11,12,13},{21,22,23}};
	static uint8_t arr3D[2][1][2] = {{{111,112}},{{211,212}}};
	return InputCommon("InputArrays", 0xD91C706D, Inputs(Input(5, "Hello"), Input(7, L"World !"),
		Input(countof(arr1D), arr1D), Input(countof(arr2D), arr2D), Input(countof(arr3D), arr3D)));
}


bool TestBasicTypes::OutputNumbers()
{
	int8_t v1; uint8_t v2; 
	int16_t v3; uint16_t v4; 
	int32_t v5; uint32_t v6; 
	int64_t v7; uint64_t v8; 
	return OutputCommonStart("OutputNumbers", "a=119; return -a,a,-a^2,a^2,-a^4,a^4,-a^7,a^7", Outputs(v1,v2,v3,v4,v5,v6,v7,v8)) &&
		OutputCommonEnd(0x55550B71, "{%d,%d,%d,%d,%d,%u,%lld,%llu}", v1,v2,v3,v4,v5,v6,v7,v8);
}

bool TestBasicTypes::OutputOtherScalars()
{
	bool bool1, bool2; 
	const char* str1; const void* ptr = NULL;
	char str2[10];
	size_t strsize = countof(str2), ptrsize;
	lua_State* thread;
	lua_CFunction fct;

	return OutputCommonStart("OutputOtherScalars",
			"return true, false, 'Hello', nil, 'World', io.stdin, coroutine.create(function()end), print", 
			Outputs(bool1, bool2, str1, nil, Output(strsize, str2), Output(ptr, ptrsize), thread, fct)) &&
		OutputCommonEnd(0x758DCF52, "{%hd,%hd,'%s',%d:'%s',%d:%d,%d}", 
			bool1, bool2, str1, strsize, str2, sizeof(void*)-ptrsize, ptr!=NULL, thread!=NULL, fct!=NULL);
}

namespace lua {
template<> void Output::GetValue<TestBasicTypes::tMessage>(lua_State* L, int idx) const
{
	TestBasicTypes::tMessage* msg = (TestBasicTypes::tMessage*) PointerValue;
	luaL_checktype(L, idx, LUA_TTABLE);
	lua_getfield(L, idx, "Number");
	msg->Number = luaL_checkint(L, -1);
	lua_getfield(L, idx, "Text");
	msg->Text = luaL_checkstring(L, -1);
	lua_pop(L, 2);
}}

bool TestBasicTypes::OutputCustom()
{
	tMessage msg;
	return OutputCommonStart("OutputCustom", "return {Number=3, Text='Hello World!'}", Outputs(msg)) &&
		OutputCommonEnd(0x64D4696F, "{Number=%d,Text='%s'}", msg.Number, msg.Text);
}

bool TestBasicTypes::OutputArrays()
{
	int16_t arr1D[4];
	uint32_t arr2D[2][3];
	uint8_t arr3D[2][1][2];
	size_t arr1DLen = countof(arr1D), arr2DLen = countof(arr2D), arr3DLen = countof(arr3D);
	return OutputCommonStart("OutputArrays", "return {1,2,3,4}, {{11,12,13},{21,22,23}}, {{{111,112}},{{211,212}}}", 
			Outputs(Output(arr1DLen, arr1D), Output(arr2DLen, arr2D), Output(arr3DLen, arr3D))) &&
		OutputCommonEnd(0x5FD6A720, "%d:{%d,%d,%d,%d},%d:{{%u,%u,%u},{%u,%u,%u}},%d:{{{%d,%d}},{{%d,%d}}}", 
			arr1DLen, arr1D[0], arr1D[1], arr1D[2], arr1D[3],
			arr2DLen, arr2D[0][0], arr2D[0][1], arr2D[0][2], arr2D[1][0], arr2D[1][1], arr2D[1][2],
			arr3DLen, arr3D[0][0][0], arr3D[0][0][1], arr3D[1][0][0], arr3D[1][0][1]);
}

int main(int /*argc*/, char* /*argv*/[])
{
	TestBasicTypes test(false);
	return test.All() ? 0 : 1;
}


