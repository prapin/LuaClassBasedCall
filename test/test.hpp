

typedef const char* PSTRING;
#include <stdint.h>

class Test
{
public:
	Test(bool fverbose);
	virtual bool All() = 0;
protected:
	bool Report(bool result, PSTRING test_name, PSTRING error_msg, PSTRING test_msg=NULL);
	bool Report(PSTRING test_name, PSTRING error_msg, PSTRING test_msg=NULL) { return Report(error_msg == NULL, test_name, error_msg, test_msg); }
	bool InputCommon(PSTRING testname, uint32_t crc_ref, const lua::Inputs& inputs);
	bool OutputCommonStart(PSTRING testname, PSTRING script, const lua::Outputs& outputs);
	bool OutputCommonEnd(uint32_t crc_ref, PSTRING format, ...);

	uint32_t ComputeCrc(const uint8_t* buf, size_t len);
	void MakeCrcTable();
	
	lua::Lua Lua;
	PSTRING LastTestName;
	uint32_t CrcTable[256];
	bool fVerbose;	
	uint16_t PassedCnt;
	uint16_t FailedCnt;
};
