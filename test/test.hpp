
typedef const char* PSTRING;
#include <stdint.h>

class Test
{
public:
	Test(bool fverbose);
	bool All();


	struct tMessage
	{
		int Number;
		const char* Text;
	};
protected:
	bool Report(bool result, PSTRING test_name, PSTRING error_msg, PSTRING test_msg=NULL);
	bool Report(PSTRING test_name, PSTRING error_msg, PSTRING test_msg=NULL) { return Report(error_msg == NULL, test_name, error_msg, test_msg); }
	bool InputCommon(PSTRING testname, uint32_t crc_ref, const lua::Inputs& inputs);
	bool OutputCommonStart(PSTRING testname, PSTRING script, const lua::Outputs& outputs);
	bool OutputCommonEnd(uint32_t crc_ref, PSTRING format, ...);
private:
	uint32_t ComputeCrc(const uint8_t* buf, size_t len);
	void MakeCrcTable();

	bool InputNumbers();
	bool InputOtherScalars();
	bool InputCustom();
	bool InputArrays();
	bool OutputNumbers();
	bool OutputOtherScalars();
	bool OutputCustom();
	bool OutputStrings();
	
	lua::Lua Lua;
	PSTRING LastTestName;
	uint32_t CrcTable[256];
	uint16_t PassedCnt;
	uint16_t FailedCnt;
	bool fVerbose;	
};
