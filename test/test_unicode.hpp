

class TestUnicode : public Test
{
public:
	TestUnicode(int argc, const PSTRING argv[]) : Test(argc, argv) {}
	virtual bool All();

private:
	bool InputStrings();
	bool OutputNumbers();
	bool OverLongUTF8();
};
