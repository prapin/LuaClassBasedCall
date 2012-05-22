// Configuration flags: TinyXML ON
#define LCBC_USE_MFC 0
#define LCBC_USE_CSL 0
#define LCBC_USE_TINYXML 1
#define LCBC_USE_WIDESTRING 0
#define LCBC_USE_EXCEPTIONS 0

#define _CRT_SECURE_NO_WARNINGS
#include "lgencall.hpp"
#include "test.hpp"
#include "test_tinyxml.hpp"
#include <sstream>

using namespace lua;

bool TestTinyXml::All()
{
	InputNode();
	OutputNode();
	//CopyFile();
	return FailedCnt == 0;
}

bool TestTinyXml::InputNode()
{
	TiXmlDocument doc;
	doc.Parse("<?xml version=\"1.0\" standalone=\"yes\"?><!--comm--><!DOCTYPE test><doc><tag1 A=1 B=2 C=D/>hello&lt;<tag2>&lt;World</tag2><!-- comment --></doc>");
	return InputCommon("InputNode", 0xb45e98ab, Inputs(doc));
}

bool TestTinyXml::OutputNode()
{
	TiXmlDocument doc;
	PSTRING error = Lua.PCall("return {'<?xml version=\"1.0\">','<!DOCTYPE test>',{[0]='tag', A=1,B=2,C='D','text','<!-- comment-->',' <hello>'}}", Outputs(doc));
	if(error)
		return Report("OutputNode", error);
	stringstream str;
	str << doc;
	return CrcAndReport("OutputNode", 0xd4c4ff8a, str.str().c_str());
}

bool TestTinyXml::CopyFile()
{
	TiXmlDocument indoc, outdoc;
	indoc.LoadFile("input.xml");
	PSTRING error = Lua.PCall("return ...", indoc, outdoc);
	if(error)
		return Report("CopyFile", error);
	outdoc.SaveFile("output.xml");
	return true;
};

int main(int argc, const PSTRING argv[])
{
	TestTinyXml test(argc, argv);
	return test.All() ? 0 : 1;
}
