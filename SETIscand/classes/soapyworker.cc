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
	int ping	= DataMgr::instance().blockFor(elems, _sdr->sampleBytes());
	int pong	= DataMgr::instance().blockFor(elems, _sdr->sampleBytes());

	void *pingBuffers[]		= {DataMgr::instance().asUint8(ping)};
	void *pongBuffers[]		= {DataMgr::instance().asUint8(pong)};


	/**************************************************************************\
	|* Configure the stream
	\**************************************************************************/
	SoapySDR::Stream *rx = _sdr->rxStream();

	/**************************************************************************\
	|* Enter the loop
	\**************************************************************************/
	bool isPing = true;
	while (_isActive)
		{
		void **buffers = isPing ? pingBuffers : pongBuffers;
		int flags = 0;
		long long time_ns = 0;

		// Read the data
		int samples = _sdr->waitForData(rx, buffers, elems, flags, time_ns);
		emit dataAvailable(isPing ? ping : pong,
						   samples,
						   _sdr->maxValue(),
						   _sdr->sampleBytes());

		// Cycle around with the next buffer
		isPing = !isPing;
		}
	}

/******************************************************************************\
|* Stop sampling from the SOAPY device
\******************************************************************************/
void SoapyWorker::stopSampling(void)
	{

	}
