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
	InputArrays();
	InputHash();
	InputOther();
	OutputArrays();
	OutputStringArrays();
	OutputHash();
	OutputQueues();
	OutputOther();
	return FailedCnt == 0;
}

bool TestCSL::InputStrings()
{
	string s1("P2\0P3", 5);
	wstring s2(L"P4\0P5", 5);
	return InputCommon("InputStrings", 0xc207dadc, Inputs(s1, s2));
}

bool TestCSL::InputArrays()
{
	vector<const char*> v1; v1.push_back("s7"); v1.push_back("s8"); 
	list<const wchar_t*, allocator<const wchar_t*> > v2; v2.push_back(L"s9"); v2.push_back(L"s10"); 
	deque<int> v3; v3.push_back(8);
	return InputCommon("InputArrays", 0x47a2b000, Inputs(v1, v2, v3));
}

bool TestCSL::InputHash()
{
	map<const char*,int > v1; v1["s1"]=1;
	set<float, greater<float>, allocator<float> > v2; v2.insert(2.5f), v2.insert(9.5f);
	multiset<short> v3; v3.insert(1); v3.insert(2); v3.insert(1); 
	return InputCommon("InputHash", 0x47a2b000, Inputs(v1, v2, v3));
}

bool TestCSL::InputOther()
{
	pair<int, PSTRING> v1(2, "Hello");
	return InputCommon("InputOther", 0x47a2b000, Inputs(v1));
}

bool TestCSL::OutputArrays()
{
	vector<short> v1;
	list<char> str; 
	deque<double> v3;
	return OutputCommonStart("OutputArrays", "return {1,2,3,4},{72,101,108,108,111,0},{8,9}", 
			Outputs(v1, str, v3)) &&
		OutputCommonEnd(0x56dfd160, "%d:{%d,%d,%d,%d},%d,%d", 
			v1.size(), v1[0], v1[1],v1[2], v1[3], str.size(),v3.size());
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

bool TestCSL::OutputHash()
{
	map<const char*,int> v1;
	set<float> v2;
	multiset<short> v3;
	return OutputCommonStart("OutputHash", "return {S1=2,S2=1},{1,1},{3,2}", 
			Outputs(v1, v2, v3)) &&
		OutputCommonEnd(0x56dfd160, "%d,%d,%d", 
			v1.size(), v2.size(),v3.size());
}

bool TestCSL::OutputQueues()
{
	queue<int> v1; 
	stack<wstring> v2;
	priority_queue<char> v3;
	return OutputCommonStart("OutputQueues", "return {1,2,3},{4,5},{6,7,8}", 
			Outputs(v1, v2, v3)) &&
		OutputCommonEnd(0x56dfd160, "%d,%d,%d", 
			v1.size(), v2.size(),v3.size());
}

bool TestCSL::OutputOther()
{
	pair<int, string> v1;
	return OutputCommonStart("OutputOther", "return {1,2}", 
			Outputs(v1)) &&
		OutputCommonEnd(0x56dfd160, "{%d,%s}", 
		v1.first, v1.second.c_str());
}


int main(int /*argc*/, char* /*argv*/[])
{
	TestCSL test(true);
	return test.All() ? 0 : 1;
}


