

class TestMFC : public Test
{
public:
	TestMFC(bool verbose) : Test(verbose) {}
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
	bool OutputSimpleValues();
	bool Serialization();
};
