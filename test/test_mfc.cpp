// Configuration flags: C++ Standard Library ON
#define LCBC_USE_MFC 1
#define LCBC_USE_CSL 0
#define LCBC_USE_WIDESTRING 3
#define LCBC_USE_EXCEPTIONS 0

#define _CRT_SECURE_NO_WARNINGS
#include "lgencall.hpp"
#include "test.cpp"
#include "test_mfc.hpp"

using namespace lua;
using namespace std;

bool TestMFC::All()
{
	InputStrings();
	InputStringList();
	InputArrays();
	OutputArrays();
	OutputStringArrays();
	return FailedCnt == 0;
}

bool TestMFC::InputStrings()
{
	CStringA s1("P2\0P3", 5);
	CStringW s2(L"P4\0P5", 5);
	return InputCommon("InputStrings", 0xc207dadc, Inputs(s1, s2));
}

bool TestMFC::InputArrays()
{
	CByteArray v1; v1.Add(1); v1.Add(2);
	CDWordArray v2; v2.Add(3); v2.Add(400); v2.Add(5);
	CPtrArray v3; v3.Add(NULL);
	CUIntArray v4; v4.Add(6); v4.Add(7);
	CWordArray v5; v5.Add(8);
	CObArray v6; v6.Add(&v1);

	return InputCommon("InputArrays", 0x3468A59E, Inputs(v1, v2, v3, v4, v5, v6));
}

bool TestMFC::InputStringList()
{
	CArray<CStringA> v1; v1.Add("s7"); v1.Add("s8"); 
	CStringArray v2; v2.Add(L"s9"); v2.Add(L"s10"); 
	return InputCommon("InputStringList", 0x47a2b000, Inputs(v1, v2));
}

bool TestMFC::OutputArrays()
{
	CByteArray v1;
	CDWordArray v2;
	CPtrArray v3;
	CUIntArray v4;
	CWordArray v5;
	CObArray v6;
	return OutputCommonStart("OutputArrays", "return {1,2,3,4},{5,6},{io.stdin},{10},{11,12},{}", 
			Outputs(v1, v2, v3, v4, v5, v6)) &&
			OutputCommonEnd(0xE4EA5C26, "%d:{%d,%d,%d,%d},%d:{%d,%d},%d:{%d},%d:{%d},%d:{%d,%d},%d:{}", 
			v1.GetSize(), v1[0], v1[1],v1[2], v1[3], 
			v2.GetSize(), v2[0], v2[1], 
			v3.GetSize(), v3[0] != NULL,
			v4.GetSize(), v4[0], 
			v5.GetSize(), v5[0], v5[1],
			v6.GetSize());
	return true;
}

bool TestMFC::OutputStringArrays()
{
	CArray<const char*> str1;
	CArray<const wchar_t*> str2;
	CArray<CStringA> str3;
	CArray<CStringW> str4;
	CStringArray str5;
	return OutputCommonStart("OutputStringArrays", "return {1,2,3},{44,55,66},{10,9,8,7},{6,5},{4,3}",
			Outputs(str1, str2, str3, str4, str5)) &&
		OutputCommonEnd(0xDA0F9DEB, "%d:{%s,%s,%s},%d:{%S,%S,%S},%d:{%s,%s,%s,%s},%d:{%S,%S},%d:{%c,%c}",
			str1.GetSize(), str1[0], str1[1], str1[2],
			str2.GetSize(), str2[0], str2[1], str2[2],
			str3.GetSize(), str3[0], str3[1], str3[2], str3[3],
			str4.GetSize(), str4[0], str4[1],
			str5.GetSize(), str5[0][0], str5[1][0]);
}

int main(int /*argc*/, char* /*argv*/[])
{
	TestMFC test(true);
	return test.All() ? 0 : 1;
}


