

class TestCSL : public Test
{
public:
	TestCSL(bool verbose) : Test(verbose) {}
	virtual bool All();

private:
	bool InputStrings();
	bool InputArrays();
	bool InputHash();
	bool InputQueues();
	bool InputOther();
	bool OutputArrays();
	bool OutputStringArrays();
	bool OutputHash();
	bool OutputQueues();
	bool OutputOther();
};
