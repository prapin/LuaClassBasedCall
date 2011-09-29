

class TestMFC : public Test
{
public:
	TestMFC(int argc, const PSTRING argv[]) : Test(argc, argv) {}
	virtual bool All();

private:
	bool InputStrings();
	bool InputArrays();
	bool InputStringArray();
	bool InputLists();
	bool InputMaps();
	bool InputSimpleValues();
	bool OutputArrays();
	bool OutputStringArrays();
	bool OutputLists();
	bool OutputMaps();
	bool OutputSimpleValues();
	bool Serialization();
};
