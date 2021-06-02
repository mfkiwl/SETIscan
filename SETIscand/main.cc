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

	/**************************************************************************\
	|* Prevent copying data when transferring from thread to thread
	\**************************************************************************/
	DataMgr& dmgr = DataMgr::instance();

	SoapyIO sio(&a);
	sio.startWorker();

	return a.exec();
	}
