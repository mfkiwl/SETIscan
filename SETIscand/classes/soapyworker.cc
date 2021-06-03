#include <unistd.h>

#include <SoapySDR/Device.hpp>

#include "datamgr.h"
#include "soapyio.h"
#include "soapyworker.h"

/******************************************************************************\
|* Constructor
\******************************************************************************/
SoapyWorker::SoapyWorker(QObject *parent)
			:QObject(parent)
			,_isActive(false)
	{}

/******************************************************************************\
|* Start sampling from the SOAPY device
\******************************************************************************/
void SoapyWorker::startSampling(void)
	{
	_isActive = true;

	/**************************************************************************\
	|* Obtain a sample buffer from the data manager
	\**************************************************************************/
	int elems	= _sdr->sampleRate();
	int handle	= DataMgr::instance().blockFor(elems, _sdr->sampleBytes());

	void *buffers[] = {DataMgr::instance().asUint8(handle)};

	/**************************************************************************\
	|* Configure the stream
	\**************************************************************************/
	SoapySDR::Stream *rx = _sdr->rxStream();

	/**************************************************************************\
	|* Enter the loop
	\**************************************************************************/
	while (_isActive)
		{
		int flags = 0;
		long long time_ns = 0;
		int ret = _sdr->waitForData(rx, buffers, elems, flags, time_ns);
		fprintf(stderr, "ret:%d, flags:%d, elems: %d time:%lld\n",
				ret, flags, elems, time_ns);
		}
	}

/******************************************************************************\
|* Stop sampling from the SOAPY device
\******************************************************************************/
void SoapyWorker::stopSampling(void)
	{

	}
