// Configuration flags: Qt and exceptions ON
#define LCBC_USE_MFC 0
#define LCBC_USE_CSL 0
#define LCBC_USE_QT 1
#define LCBC_USE_WIDESTRING 0
#define LCBC_USE_EXCEPTIONS 1

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
	QString code1("return 'Hello world'");
	const char* v1;
	QString error1 = L.PCall(code1, Output(v1));
	QString error2 = L.PCall(Script(QString(","), QString("Invalid snippet")));
	LastTestName = "QStringEnv";
	L.ECall(File(QString("dumper.lua")));
	int v2 = L.TCall<int>(Global(QString("select")), 2, 1, 2, 3);
	return OutputCommonEnd(0, "%s,%S,%S,%d", v1, error1.utf16(),error2.utf16(), v2);
}

int main(int argc, const PSTRING argv[])
{
	TestQt test(argc, argv);
	return test.All() ? 0 : 1;
}


