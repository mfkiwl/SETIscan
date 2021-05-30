#ifndef TESTABLE_H
#define TESTABLE_H

class Testable
	{
	public:
		typedef enum
			{
			TEST_PASS		= 0,
			TEST_FAIL
			} TestResult;

		virtual int numTests(void) = 0;
		virtual TestResult runTest(int idx) = 0;
		virtual const char * testClassName(void) = 0;
	};

#endif // TESTABLE_H
