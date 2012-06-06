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
	InputTime();
	InputString();
	InputGeometric();
	OutputTime();
	OutputString();
	OutputGeometric();
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
	return OutputCommonEnd(0x610E1532, "%s,%S,%S,%d", v1, error1.utf16(),error2.utf16(), v2);
}

bool TestQt::InputTime()
{
	QDate v1(2012, 1, 1);
	QTime v2(13, 30, 40);
	QDateTime v3(v1, v2);
	return InputCommon("InputTime", 0x22605EBF, Inputs(v1, v2, v3));
}

bool TestQt::InputString()
{
	QString v1("Hello");
	QLatin1String v2("World");
	QByteArray v3("P\0Q", 3);
	QChar v4('/');
	QLatin1Char v5('&');
	return InputCommon("InputString", 0xE62DCA58, Inputs(v1, v2, v3, v4, v5));
}

bool TestQt::InputGeometric()
{
	QPoint v1(3,-4);
	QPointF v2(3.25,-4.5);
	QLine v3(1,2,3,4);
	QLineF v4(1.5,2.25,3.75,4);
	QRect v5(1,2,3,4);
	QRectF v6(1.5,2.25,3.75,4);
	QSize v7(10,9);
	QSizeF v8(10.5,9.5);
	return InputCommon("InputGeometric", 0xC0CF0CD3, Inputs(v1,v2,v3,v4,v5,v6,v7,v8));
}

bool TestQt::OutputTime()
{
	QDate v1;
	QTime v2;
	QDateTime v3;

	return OutputCommonStart("OutputTime", "return 2455928,48640,1325421040",
			Outputs(v1, v2, v3)) &&
		OutputCommonEnd(0x6DDE65ED, "%d,%d,%d", v1.toJulianDay(), -v2.secsTo(QTime()), v3.toTime_t());
}

bool TestQt::OutputString()
{
	QString v1;
	QByteArray v2;
	return OutputCommonStart("OutputString", "return 'Hello','World'",
			Outputs(v1, v2)) &&
		OutputCommonEnd(0x3DD5712D, "%S,%s", v1.utf16(), (const char*)v2);
}

bool TestQt::OutputGeometric()
{
	QPoint v1;
	QPointF v2;
	QLine v3;
	QLineF v4;
	QRect v5;
	QRectF v6;
	QSize v7;
	QSizeF v8;
	return OutputCommonStart("OutputGeometric", 
		"return {x=3,y=-4},{x=3.25,y=-4.5},"
		"{x1=1,x2=3,y1=2,y2=4},{x1=1.5,x2=3.75,y1=2.25,y2=4},"
		"{height=4,width=3,x=1,y=2},{height=4,width=3.75,x=1.5,y=2.25},"
		"{height=9,width=10},{height=9.5,width=10.5}",
			Outputs(v1,v2,v3,v4,v5,v6,v7,v8)) &&
		OutputCommonEnd(0x6C2EC8DC, "{%d,%d},{%g,%g},"
			"{%d,%d,%d,%d},{%g,%g,%g,%g},"
			"{%d,%d,%d,%d},{%g,%g,%g,%g},"
			"{%d,%d},{%g,%g}",
			v1.x(), v1.y(), v2.x(), v2.y(), 
			v3.x1(), v3.y1(), v3.x2(), v3.y2(),
			v4.x1(), v4.y1(), v4.x2(), v4.y2(),
			v5.x(), v5.y(), v5.width(), v5.height(),
			v6.x(), v6.y(), v6.width(), v6.height(),
			v7.width(), v7.height(), v8.width(), v8.height());
}
int main(int argc, const PSTRING argv[])
{
	TestQt test(argc, argv);
	return test.All() ? 0 : 1;
}


