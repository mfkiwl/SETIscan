#include <unistd.h>

#include "soapyworker.h"

/******************************************************************************\
|* Constructor
\******************************************************************************/
SoapyWorker::SoapyWorker(QObject *parent) : QObject(parent)
	{

	}

/******************************************************************************\
|* Start sampling from the SOAPY device
\******************************************************************************/
void SoapyWorker::startSampling(void)
	{
	while (true)
		{
		sleep(1);
		fprintf(stderr, "check!\n");
		}
	}

/******************************************************************************\
|* Stop sampling from the SOAPY device
\******************************************************************************/
void SoapyWorker::stopSampling(void)
	{

	}
