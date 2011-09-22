

class TestCSL : public Test
{
public:
	TestCSL(bool verbose) : Test(verbose) {}
	virtual bool All();

private:
	bool InputStrings();
	bool InputStringList();
	bool OutputArrays();
	bool OutputStringArrays();
};
