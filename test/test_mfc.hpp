

class TestMFC : public Test
{
public:
	TestMFC(bool verbose) : Test(verbose) {}
	virtual bool All();

private:
	bool InputStrings();
	bool InputArrays();
	bool InputStringList();
	bool InputSimpleValues();
	bool OutputArrays();
	bool OutputStringArrays();
	bool OutputSimpleValues();
	bool Serialization();
};
