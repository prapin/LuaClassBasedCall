

class TestSyntax : public Test
{
public:
	TestSyntax(int argc, const PSTRING argv[]) : Test(argc, argv) {}
	virtual bool All();

private:
	bool UCall();
	bool PCall();
	bool ECall();
	bool PShift();
	bool EShift();
};
