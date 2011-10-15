// Configuration flags: TinyXML ON
#define LCBC_USE_MFC 0
#define LCBC_USE_CSL 0
#define LCBC_USE_TINYXML 1
#define LCBC_USE_WIDESTRING 0
#define LCBC_USE_EXCEPTIONS 0
#define TIXML_USE_STL

#define _CRT_SECURE_NO_WARNINGS
#include "lgencall.hpp"
#include "test.hpp"
#include "test_tinyxml.hpp"

using namespace lua;

bool TestTinyXml::All()
{
	InputNode();
	return FailedCnt == 0;
}

bool TestTinyXml::InputNode()
{
	TiXmlDocument doc;
	doc.Parse("<?xml version=\"1.0\" standalone=\"yes\"?><!--comm--><!DOCTYPE test><doc><tag1 A=1 B=2 C=D/>hello&lt;<tag2>&lt;World</tag2><!-- comment --></doc>");
	return InputCommon("InputNode", 0xb45e98ab, Inputs(&doc));
}

int main(int argc, const PSTRING argv[])
{
	TestTinyXml test(argc, argv);
	return test.All() ? 0 : 1;
}
