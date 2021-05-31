#include <QCoreApplication>

#include "datamgr.h"
#include "tester.h"
#include "soapyio.h"

int main(int argc, char *argv[])
	{
	QCoreApplication a(argc, argv);

	DataMgr& dmgr = DataMgr::instance();

	Tester tester;
	tester.duts().append(&dmgr);
	tester.test();

	SoapyIO sio;
	//return a.exec();
	}
