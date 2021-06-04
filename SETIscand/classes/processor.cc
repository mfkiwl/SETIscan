#include <complex>

#include "config.h"
#include "datamgr.h"
#include "processor.h"
#include "soapyio.h"

/******************************************************************************\
|* Constructor
\******************************************************************************/
Processor::Processor(Config& cfg, QObject *parent)
		  : QObject(parent)
		  ,_cfg(cfg)
		  ,_sio(nullptr)
		  ,_fftSize(0)
		  ,_aggregate(-1)
	{
	}

/******************************************************************************\
|* We got data back
\******************************************************************************/
void Processor::dataReceived(int buffer)
	{
	fprintf(stderr, "Got buffer %d\n", buffer);
	}




/******************************************************************************\
|* Initialise
\******************************************************************************/
void Processor::init(SoapyIO *sio)
	{
	_sio		= sio;
	_fftSize	= _cfg.fftSize();
	_allocate();
	}

/******************************************************************************\
|* Set up the buffers
\******************************************************************************/
void Processor::_allocate(void)
	{
	DataMgr &dmgr = DataMgr::instance();

	_aggregate	= dmgr.blockFor(_fftSize, sizeof(double));;
	_fft		= dmgr.blockFor(_fftSize, sizeof(std::complex<double>));

	}
