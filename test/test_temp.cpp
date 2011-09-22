#if 0
bool Test::OutputStrings()
{
	const TCHAR *str1;
	TCHAR str2[20];
	size_t str2_len = countof(str2);
	char str3[10];
	size_t str3_len = countof(str3);
	unsigned char data[6];
	size_t len = sizeof(data);
	const wchar_t* wstr;
	return OutputCommonStart("OutputStrings", "return 'Hello', ' Wor', 'ld!', '\\0\\5\\200\\0', 'Unicode'",
		Outputs(str1, Output(str2_len, str2), Output(str3_len, str3), Output(len, (char*)data), wstr)) &&
		OutputCommonEnd(0x4C218ADE, "%s\ndata (%d bytes): %02X %02X %02X %02X %02X\n",
			str3, len, data[0],data[1],data[2],data[3],data[4]);
	_tprintf(_T("%s%s"), str1, str2);		
	printf("wstr = %S\n", wstr);
#if 0
	string s1;
	wstring s2;
	//CStringA s3;
	//CStringW s4;
	L.UCall("a='Hello\\0World\\0' return a,a", Outputs(s1, s2));
	printf("%d %d\n", s1.size(), s2.size());
#endif
}
#if 0
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
#endif
#endif
