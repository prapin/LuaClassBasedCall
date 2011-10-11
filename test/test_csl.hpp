

class TestCSL : public Test
{
public:
	TestCSL(int argc, const PSTRING argv[]) : Test(argc, argv) {}
	virtual bool All();

private:
	bool InputStrings();
	bool InputArrays();
	bool InputHash();
	bool InputQueues();
	bool InputValArrays();
	bool InputOther();
	bool OutputArrays();
	bool OutputStringArrays();
	bool OutputHash();
	bool OutputQueues();
	bool OutputValArrays();
	bool OutputOther();
	bool OutputComplex();
};
