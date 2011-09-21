
typedef const char* PSTRING;
#include <stdint.h>
using namespace lua;

class Test
{
public:
	Test(bool fverbose);
	bool Report(bool result, PSTRING error_msg, PSTRING test_msg);
	bool Report(PSTRING error_msg, PSTRING test_msg) { return Report(error_msg == NULL, error_msg, test_msg); }
	
	bool InputNumbers();
private:
	bool InputCommon(uint32_t crc_ref, const Inputs& inputs);
	uint32_t ComputeCrc(const uint8_t* buf, size_t len);
	void MakeCrcTable();
	
	lua::Lua Lua;
	uint32_t CrcTable[256];
	uint16_t PassedCnt;
	uint16_t FailedCnt;
	bool fVerbose;	
};
