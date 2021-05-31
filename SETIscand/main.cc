#include <QCoreApplication>

#include "config.h"
#include "constants.h"
#include "datamgr.h"
#include "tester.h"
#include "soapyio.h"

int main(int argc, char *argv[])
	{
	QCoreApplication a(argc, argv);

	/**************************************************************************\
	|* Set up the settings for the application
	\**************************************************************************/
	QCoreApplication::setOrganizationName(ORG_NAME);
	QCoreApplication::setOrganizationDomain(ORG_DOMAIN);
	QCoreApplication::setApplicationName(APP_NAME);
	QCoreApplication::setApplicationVersion(APP_VERSION);

	/**************************************************************************\
	|* Set up the configuration from both settings and commandline
	\**************************************************************************/
	Config &cfg = Config::instance();


	DataMgr& dmgr = DataMgr::instance();

	Tester tester;
	tester.duts().append(&dmgr);
	tester.test();

	SoapyIO sio;
	//return a.exec();
	}
