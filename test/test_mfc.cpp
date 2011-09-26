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
	InputStringArray();
	InputArrays();
	InputLists();
	InputMaps();
	InputSimpleValues();
	OutputArrays();
	OutputStringArrays();
	OutputLists();
	OutputMaps();
	OutputSimpleValues();
	Serialization();
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

bool TestMFC::InputLists()
{
	CList<int,int> v1; v1.AddTail(1); v1.AddTail(2); v1.AddTail(3);
	CList<CString,const CString&> v2; v2.AddTail("Hello"); v2.AddTail("World");
	CPtrList v3; v3.AddHead((void*)NULL);
	CByteArray obj;
	CObList v4; v4.AddHead(&obj);
	CStringList v5; v5.AddTail("Foo"); v5.AddTail("bar"); v5.AddTail("Baz"); 
	return InputCommon("InputLists", 0x163784A4, Inputs(v1, v2, v3, v4, v5));
}

bool TestMFC::InputStringArray()
{
	CArray<CStringA> v1; v1.Add("s7"); v1.Add("s8"); 
	CStringArray v2; v2.Add(L"s9"); v2.Add(L"s10"); 
	return InputCommon("InputStringList", 0x47a2b000, Inputs(v1, v2));
}

bool TestMFC::InputMaps()
{
	CMap<int,int,PSTRING,PSTRING> v1; v1.SetAt(1, "V1"); v1.SetAt(2, "V2");
	CMapWordToPtr v2; v2.SetAt(3, NULL);
	CMapPtrToWord v3; v3.SetAt(NULL, 9);
	CMapPtrToPtr v4; v4.SetAt(NULL, NULL);
	CByteArray obj;
	CMapWordToOb v5; v5.SetAt(4, &obj);
	CMapStringToPtr v6; v6.SetAt(_T("Hello"), NULL);
	CMapStringToOb v7; v7.SetAt(_T("World"), &obj);
	CMapStringToString v8; v8.SetAt(_T("Foo"), _T("Bar"));

	return InputCommon("InputMaps", 0x85405EA7, Inputs(v1, v2, v3, v4, v5, v6, v7, v8));
}

bool TestMFC::InputSimpleValues()
{
	CPoint v1(1,2);
	CRect v2(3,4,5,6);
	CSize v3(7,8);
	CTime v4(9);
	CTimeSpan v5(10);
	return InputCommon("InputSimpleValues", 0xF4EB9FC9, Inputs(v1, v2, v3, v4, v5));
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

bool TestMFC::OutputLists()
{
	CList<BYTE,BYTE> v1;
	CPtrList v2;
	CObList v3;
	CStringList v4;
	return OutputCommonStart("OutputLists", "return {1,2,3,4},{io.stdin},{},{'Hello'}", 
			Outputs(v1, v2, v3, v4)) &&
		OutputCommonEnd(0xE4EA5C26, "%d:{%d,...},%d:{%d},%d:{},%d:{%S}", 
			v1.GetCount(), v1.GetHead(), 
			v2.GetCount(), v2.GetHead() != NULL,
			v3.GetCount(),  
			v4.GetCount(), v4.GetHead());
}

bool TestMFC::OutputMaps()
{
	CMap<int,int,PSTRING,PSTRING> v1;
	CMapWordToPtr v2;
	CMapPtrToWord v3;
	CMapPtrToPtr v4;
	CMapWordToOb v5;
	CMapStringToPtr v6;
	CMapStringToOb v7;
	CMapStringToString v8;
	return OutputCommonStart("OutputMaps", "p=io.stdin; return {1,2},{p},{[p]=5},{[p]=p},{},{hello=p},{},{foo='baz'}", 
			Outputs(v1, v2, v3, v4, v5, v6, v7, v8)) &&
		OutputCommonEnd(0x5A8DE320, "%d,%d,%d,%d,%d,%d,%d,%d", 
			v1.GetCount(), v2.GetCount(), v3.GetCount(), v4.GetCount(),
			v5.GetCount(), v6.GetCount(), v7.GetCount(), v8.GetCount());
}

bool TestMFC::OutputSimpleValues()
{
	CPoint v1;
	CRect v2;
	CSize v3;
	CTime v4;
	CTimeSpan v5;
	return OutputCommonStart("OutputSimpleValues", "return {x=1,y=2},{left=3,right=4,top=5,bottom=6},{cx=7,cy=8},9,10",
			Outputs(v1, v2, v3, v4, v5)) &&
		OutputCommonEnd(0xD110F687, "{x=%d,y=%d},{left=%d,right=%d,top=%d,bottom=%d},{cx=%d,cy=%d},%d,%d",
			v1.x, v1.y, 
			v2.left, v2.right, v2.top, v2.bottom,
			v3.cx, v3.cy,
			(int)v4.GetTime(), (int)v5.GetTimeSpan());

}

bool TestMFC::Serialization()
{
	CStringArray v1; v1.Add("s7"); v1.Add("s8"); 
	CObject *i1=&v1, *o1;
	PSTRING error = Lua.PCall("return ...", Inputs(i1), Outputs(o1));
	bool res = Report(error == NULL && o1->IsKindOf(i1->GetRuntimeClass()), "Serialization", error);
	delete o1;
	return res;
}

int main(int /*argc*/, char* /*argv*/[])
{
	TestMFC test(true);
	return test.All() ? 0 : 1;
}


