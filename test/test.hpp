

typedef const char* PSTRING;
#include <stdint.h>

class Test
{
public:
	Test(int argc, const PSTRING argv[]);
	virtual bool All() = 0;
protected:
	bool Report(bool result, PSTRING test_name, PSTRING error_msg, PSTRING test_msg=NULL);
	bool Report(PSTRING test_name, PSTRING error_msg, PSTRING test_msg=NULL) { return Report(error_msg == NULL, test_name, error_msg, test_msg); }
	bool InputCommon(PSTRING testname, uint32_t crc_ref, const lua::Inputs& inputs);
	bool OutputCommonStart(PSTRING testname, PSTRING script, const lua::Outputs& outputs);
	bool OutputCommonEnd(uint32_t crc_ref, PSTRING format, ...);
	bool CrcAndReport(PSTRING testname, uint32_t crc_ref, const char* dump);
	uint32_t ComputeCrc(const uint8_t* buf, size_t len);
	void MakeCrcTable();
	
	lua::Lua<> Lua;
	PSTRING LastTestName;
	uint32_t CrcTable[256];
	uint8_t Verbosity;
	uint16_t PassedCnt;
	uint16_t FailedCnt;
};

Test::Test(int argc, const PSTRING argv[])
:	Verbosity(1),
	PassedCnt(0),
	FailedCnt(0)
{
	for(int i=1;i<argc;i++)
	{
		if(strcmp(argv[i], "-v") == 0)
			Verbosity++;
		if(strcmp(argv[i], "-q") == 0)
			Verbosity--;
	}
	lua::WideString::SetMode<LCBC_USE_WIDESTRING>(Lua);
	MakeCrcTable();
	Report("Loading DataDumper", Lua.PCall(lua::File("dumper.lua")));
}

bool Test::Report(bool result, PSTRING test_name, PSTRING error_msg, PSTRING test_msg)
{
	if(error_msg == NULL || Verbosity < 2)
		error_msg = "";
	if(test_msg == NULL ||  Verbosity < 2)
		test_msg = "";
	if(result)
	{
		PassedCnt++;
		if(Verbosity)
			printf("[PASSED] %s %s\n", test_name, test_msg);
	}
	else
	{
		FailedCnt++;
		if(Verbosity)
			printf("[FAILED] %s %s %s\n", test_name, error_msg, test_msg);
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


bool Test::CrcAndReport(PSTRING testname, uint32_t crc_ref, const char* dump)
{
	uint32_t crc = ComputeCrc((const uint8_t*)dump, strlen(dump));
	lua_pushfstring(Lua, "%p <=> %p %s", crc, crc_ref, dump);
	return Report(crc == crc_ref, testname, "CRC missmatch", lua_tostring(Lua, -1));
}

bool Test::InputCommon(PSTRING testname, uint32_t crc_ref, const lua::Inputs& inputs)
{
	PSTRING error, dump;
	error = Lua.PCall("return DataDumper{...}", inputs, lua::Outputs(dump));
	if(error)
		return Report(testname, error);
	return CrcAndReport(testname, crc_ref, dump);
}

bool Test::OutputCommonStart(PSTRING testname, PSTRING script, const lua::Outputs& outputs)
{
	PSTRING error = Lua.PCall(script, outputs);
	if(error)
		return Report(testname, error);
	LastTestName = testname;
	return true;
}

bool Test::OutputCommonEnd(uint32_t crc_ref, PSTRING format, ...)
{
	char buffer[1000];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);
	return CrcAndReport(LastTestName, crc_ref, buffer);
}

