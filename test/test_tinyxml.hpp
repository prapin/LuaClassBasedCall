

class TestTinyXml : public Test
{
public:
	TestTinyXml(int argc, const PSTRING argv[]) : Test(argc, argv) {}
	virtual bool All();

private:
	bool InputNode();
	bool OutputNode();
	bool CopyFile();
};
