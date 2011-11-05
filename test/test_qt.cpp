// Configuration flags: C++ Standard Library ON
#define LCBC_USE_MFC 0
#define LCBC_USE_CSL 0
#define LCBC_USE_QT 1
#define LCBC_USE_WIDESTRING 1
#define LCBC_USE_EXCEPTIONS 0

#define _CRT_SECURE_NO_WARNINGS
#include "lgencall.hpp"
#include "test.hpp"
#include "test_qt.hpp"

using namespace lua;
using namespace std;

bool TestQt::All()
{
	QStringEnv();
	return FailedCnt == 0;
}

bool TestQt::QStringEnv()
{
	LuaT<QString> L(Lua);
	QString code1("return 'Hello world'"), code2(",");
	const char* res;
	QString error1 = L.PCall(code1, Output(res));
	QString error2 = L.PCall(code2);
	LastTestName = "QStringEnv";
	return OutputCommonEnd(0, "%s,%S,%S", res, error1.utf16(),error2.utf16());
}

int main(int argc, const PSTRING argv[])
{
	TestQt test(argc, argv);
	return test.All() ? 0 : 1;
}


