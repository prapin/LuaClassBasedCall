// Configuration flags: C++ Standard Library ON
#define LCBC_USE_MFC 0
#define LCBC_USE_CSL 1
#define LCBC_USE_WIDESTRING 3
#define LCBC_USE_EXCEPTIONS 0

#define _CRT_SECURE_NO_WARNINGS
#include "lgencall.hpp"
#include "test.hpp"
#include "test_csl.hpp"
#include <iostream>
#include <sstream>

using namespace lua;
using namespace std;

// Template functions to dump container values
template<class T1, class T2> ostream& operator<< (ostream& out, const pair<T1,T2>& val)
{
	out << val.first << "=" << val.second;
	return out;
}
template<class S, class T> S& serialize (S& out, const T& val)
{
	typename T::const_iterator it;
	out << "{";
	int i=0;
	for (it=val.begin(); it != val.end(); it++)
	{
		if(i++)
			out << ",";
		out << *it;
	}
	out << "}";
	return out;
}
template<class S, class T> S& serialize_queue (S& out, const T& val)
{
	myqueue<T> copy(val);
	return serialize(out, copy.get_container());
}
template<class S, class T, class A> S& operator<< (S& out, const vector<T,A>& val) { return serialize(out, val); }
template<class S, class T, class A> S& operator<< (S& out, const list<T,A>& val) { return serialize(out, val); }
template<class S, class T, class A> S& operator<< (S& out, const deque<T,A>& val) { return serialize(out, val); }
template<class S, class K, class T, class C, class A> S& operator<< (S& out, const map<K,T,C,A>& val) { return serialize(out, val); }
template<class S, class K, class T, class C, class A> S& operator<< (S& out, const multimap<K,T,C,A>& val) { return serialize(out, val); }
template<class S, class T, class C, class A> S& operator<< (S& out, const set<T,C,A>& val) { return serialize(out, val); }
template<class S, class T, class C, class A> S& operator<< (S& out, const multiset<T,C,A>& val) { return serialize(out, val); }
template<class S, class T, class C> S& operator<< (S& out, const stack<T,C>& val) { return serialize_queue(out, val); }
template<class S, class T, class C> S& operator<< (S& out, const queue<T,C>& val) { return serialize_queue(out, val); }
template<class S, class T, class C, class P> S& operator<< (S& out, const priority_queue<T,C,P>& val) { return serialize_queue(out, val); }
template<class T> string dump(const T& value)
{
	stringstream s;
	s << value;
	return s.str();
}
template<class T> wstring wdump(const T& value)
{
	wstringstream s;
	s << value;
	return s.str();
}


bool TestCSL::All()
{
	InputStrings();
	InputArrays();
	InputHash();
	InputQueues();
	InputValArrays();
	InputOther();
	OutputArrays();
	OutputStringArrays();
	OutputHash();
	OutputQueues();
	OutputValArrays();
	OutputOther();
	OutputComplex();
	return FailedCnt == 0;
}

bool TestCSL::InputStrings()
{
	string s1("P2\0P3", 5);
	wstring s2(L"P4\0P5", 5);
	return InputCommon("InputStrings", 0xD110B8B8, Inputs(s1, s2));
}

bool TestCSL::InputArrays()
{
	vector<const char*> v1; v1.push_back("s7"); v1.push_back("s8"); 
	list<const wchar_t*, allocator<const wchar_t*> > v2; v2.push_back(L"s9"); v2.push_back(L"s10"); 
	deque<wchar_t> v3; v3.push_back(L'B'); v3.push_back(L'a'); v3.push_back(L'z');
	return InputCommon("InputArrays", 0xA3AD612F, Inputs(v1, v2, v3));
}

bool TestCSL::InputHash()
{
	map<const char*,int > v1; v1["s1"]=1;
	set<float, greater<float>, allocator<float> > v2; v2.insert(2.5f), v2.insert(9.5f);
	multiset<short> v3; v3.insert(1); v3.insert(2); v3.insert(1);
	multimap<char, double> v4; for(int i=0;i<5;i++) v4.insert(pair<char, double>("Hello"[i],i+1)); 
	return InputCommon("InputHash", 0x95DB9AC5, Inputs(v1, v2, v3, v4));
}

bool TestCSL::InputQueues()
{
	queue<int> v1; v1.push(1); v1.push(2); v1.push(3); 
	stack<wstring> v2; v2.push(L"4"); v2.push(L"5"); 
	priority_queue<char> v3; v3.push('B'); v3.push('A'); v3.push('R'); 
	return InputCommon("InputQueues", 0x16D7731E, Inputs(v1, v2, v3));
}

bool TestCSL::InputValArrays()
{
	valarray<double> v1(3); v1[0] = 1.; v1[1] = 2.; v1[2] = 3.; 
	bitset<4> v2(string("0111"));
	return InputCommon("InputValArrays", 0x9E641961, Inputs(v1, v2));
}

bool TestCSL::InputOther()
{
	pair<int, PSTRING> v1(2, "Hello");
	return InputCommon("InputOther", 0x625A8B6F, Inputs(v1));
}

bool TestCSL::OutputArrays()
{
	vector<short> v1;
	list<char> v2; 
	deque<double> v3;
	return OutputCommonStart("OutputArrays", "return {1,2,3,4},{'H','e','l','l','o'},{8,9}", 
			Outputs(v1, v2, v3)) &&
		OutputCommonEnd(0xE6715D8A, "%s,%s,%s", 
			dump(v1).c_str(), dump(v2).c_str(), dump(v3).c_str());
}

bool TestCSL::OutputStringArrays()
{
	vector<const char*> str1;
	vector<const wchar_t*> str2;
	vector<string> str3;
	vector<wstring> str4;
	return OutputCommonStart("OutputStringArrays", "return {1,2,3},{44,55,66},{10,9,8,7},{6,5}",
			Outputs(str1, str2, str3, str4)) &&
		OutputCommonEnd(0x78CA7540, "%s,%S,%s,%S", dump(str1).c_str(), 
			wdump(str2).c_str(), dump(str3).c_str(), wdump(str4).c_str());
}

bool TestCSL::OutputHash()
{
	map<const char*,int> v1;
	set<float> v2;
	multiset<short> v3;
	multimap<char, double> v4;
	return OutputCommonStart("OutputHash", "return {S1=2},{1,1},{3,2},{{'A',5},{'B',1},{'B',2}}", 
			Outputs(v1, v2, v3, v4)) &&
		OutputCommonEnd(0xd319da2d, "%s,%s,%s,%s", 
			dump(v1).c_str(), dump(v2).c_str(), dump(v3).c_str(), dump(v4).c_str());
}

bool TestCSL::OutputQueues()
{
	queue<int> v1; 
	stack<wstring> v2;
	priority_queue<wchar_t> v3;
	return OutputCommonStart("OutputQueues", "return {1,2,3},{4,5},{'A','B','C'}", 
			Outputs(v1, v2, v3)) &&
		OutputCommonEnd(0x1C41A0E3, "%s,%S,%S", 
			dump(v1).c_str(), wdump(v2).c_str(), wdump(v3).c_str());
}

bool TestCSL::OutputValArrays()
{
	valarray<double> v1;
	bitset<4> v2;
	return OutputCommonStart("OutputValArrays", "return {1,2,3},{true,true,false}", 
			Outputs(v1, v2)) &&
		OutputCommonEnd(0x15AC27F8, "%d:{%g,%g,%g},%s", 
			v1.size(), v1[0], v1[1], v1[2], v2.to_string().c_str());
}

bool TestCSL::OutputOther()
{
	pair<int, string> v1;
	return OutputCommonStart("OutputOther", "return {1,2}", 
			Outputs(v1)) &&
		OutputCommonEnd(0x78D74F8B, "%s", dump(v1).c_str());
}

bool TestCSL::OutputComplex()
{
	vector<vector<int> > v1;
	deque<map<char, list<int> > > v2;
	list<deque<deque<int> > > v3;
	stack<queue<stack<int> > > v4;
	return OutputCommonStart("OutputComplex", 
		"return {{1,2},{3},{4,5,6}},{{A={7,8},B={}},"
		"{C={9}}},{{{1,2},{3},{4}},{{5}}},{{{1},{2,3}},{{4},{5},{}},{{6,7}}}", 
			Outputs(v1,v2,v3,v4)) &&
		OutputCommonEnd(0x0ECF119A, "%s,%s,%s,%s", dump(v1).c_str(), 
			dump(v2).c_str(), dump(v3).c_str(), dump(v4).c_str());
}

int main(int argc, const PSTRING argv[])
{
	TestCSL test(argc, argv);
	return test.All() ? 0 : 1;
}


