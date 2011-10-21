

class TestQt : public Test
{
public:
	TestQt(int argc, const PSTRING argv[]) : Test(argc, argv) {}
	virtual bool All();

private:
	bool QStringEnv();
};
