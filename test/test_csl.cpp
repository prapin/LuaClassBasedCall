// Configuration flags: C++ Standard Library ON
#define LCBC_USE_MFC 0
#define LCBC_USE_CSL 1
#define LCBC_USE_WIDESTRING 3
#define LCBC_USE_EXCEPTIONS 0

#define _CRT_SECURE_NO_WARNINGS
#include "lgencall.hpp"
#include "test.cpp"
#include "test_csl.hpp"

using namespace lua;
using namespace std;

bool TestCSL::All()
{
	InputStrings();
	InputStringList();
	OutputArrays();
	OutputStringArrays();
	return FailedCnt == 0;
}

bool TestCSL::InputStrings()
{
	string s1("P2\0P3", 5);
	wstring s2(L"P4\0P5", 5);
	return InputCommon("InputStrings", 0xc207dadc, Inputs(s1, s2));
}

bool TestCSL::InputStringList()
{
	vector<const char*> v1; v1.push_back("s7"); v1.push_back("s8"); 
	vector<const wchar_t*> v2; v2.push_back(L"s9"); v2.push_back(L"s10"); 
	return InputCommon("InputStringList", 0x47a2b000, Inputs(v1, v2));
}

bool TestCSL::OutputArrays()
{
	vector<short> v1;
	vector<char> str; 
	return OutputCommonStart("OutputArrays", "return {1,2,3,4},{72,101,108,108,111,0}", 
			Outputs(v1, str)) &&
		OutputCommonEnd(0x56dfd160, "%d:{%d,%d,%d,%d},%d:'%c%c%c%c%c'\n", 
			v1.size(), v1[0], v1[1],v1[2], v1[3], str.size(), str[0], str[1], str[2], str[3], str[4]);
}

bool TestCSL::OutputStringArrays()
{
	vector<const char*> str1;
	vector<const wchar_t*> str2;
	vector<string> str3;
	vector<wstring> str4;
	return OutputCommonStart("OutputStringArrays", "return {1,2,3},{44,55,66},{10,9,8,7},{6,5}",
			Outputs(str1, str2, str3, str4)) &&
		OutputCommonEnd(0xdcde586, "%d:{%s,%s,%s},%d:{%S,%S,%S},%d:{%s,%s,%s,%s},%d:{%S,%S}", 
			str1.size(), str1[0], str1[1], str1[2],
			str2.size(), str2[0], str2[1], str2[2],
			str3.size(), str3[0].c_str(), str3[1].c_str(), str3[2].c_str(), str3[3].c_str(),
			str4.size(), str4[0].c_str(), str4[1].c_str());
}

int main(int /*argc*/, char* /*argv*/[])
{
	TestCSL test(true);
	return test.All() ? 0 : 1;
}


