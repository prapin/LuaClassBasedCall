

class TestBasicTypes : public Test
{
public:
	TestBasicTypes(bool verbose) : Test(verbose) {}
	virtual bool All();

	struct tMessage
	{
		int Number;
		const char* Text;
	};
private:
	bool InputNumbers();
	bool InputOtherScalars();
	bool InputCustom();
	bool InputArrays();
	bool OutputNumbers();
	bool OutputOtherScalars();
	bool OutputCustom();
	bool OutputArrays();
};
