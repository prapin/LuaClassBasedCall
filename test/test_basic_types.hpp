

class TestBasicTypes : public Test
{
public:
	TestBasicTypes(int argc, const PSTRING argv[]) : Test(argc, argv) {}
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
	bool InOutRegistry();
};
