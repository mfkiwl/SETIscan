#include <QCoreApplication>

#include "config.h"
#include "constants.h"
#include "datamgr.h"
#include "processor.h"
#include "soapyio.h"
#include "tester.h"

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
	|* Set up the processing hierarchy
	\**************************************************************************/
	Processor processor(cfg, &a);

	/**************************************************************************\
	|* Set up the data stream
	\**************************************************************************/
	SoapyIO sio(&processor);

	/**************************************************************************\
	|* Configure the processor
	\**************************************************************************/
	processor.init(&sio);

	/**************************************************************************\
	|* Start streaming data in
	\**************************************************************************/
	sio.startWorker();

	return a.exec();
	}
