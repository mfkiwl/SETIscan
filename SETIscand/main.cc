#include <QCoreApplication>

#include "datamgr.h"
#include "tester.h"

int main(int argc, char *argv[])
	{
	QCoreApplication a(argc, argv);

	DataMgr& dmgr = DataMgr::instance();

	Tester tester;
	tester.duts().append(&dmgr);
	tester.test();

	//return a.exec();
	}
