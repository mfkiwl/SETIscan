#ifndef TESTER_H
#define TESTER_H

#include <QVector>

#include "properties.h"
#include "testable.h"

class Tester
	{
	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GET(QVector<Testable *>, duts);

	public:
		Tester();

		void test(void);
	};

#endif // TESTER_H
