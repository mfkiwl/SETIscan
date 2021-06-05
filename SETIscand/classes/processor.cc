#include <complex>

#include <QThreadPool>

#include "config.h"
#include "datamgr.h"
#include "processor.h"
#include "soapyio.h"
#include "taskfft.h"

/******************************************************************************\
|* Constructor
\******************************************************************************/
Processor::Processor(Config& cfg, QObject *parent)
		  : QObject(parent)
		  ,_cfg(cfg)
		  ,_sio(nullptr)
		  ,_fftSize(0)
		  ,_aggregate(-1)
		  ,_fft(-1)
	{
	}

/******************************************************************************\
|* We got data back
\******************************************************************************/
void Processor::dataReceived(int buffer, int samples, int max)
	{
	DataMgr &dmgr	= DataMgr::instance();
	int8_t * src8	= dmgr.asInt8(buffer);
	int16_t *src16	= dmgr.asInt16(buffer);
	double *work	= dmgr.asDouble(_work);
	double scale	= 1.0 / (double)max;

	/**************************************************************************\
	|* We expect complex (interleaved I,Q) data since we use the native format
	|* of the device, so multiply samples by 2
	\**************************************************************************/
	samples *= 2;

	/**************************************************************************\
	|* Convert the buffer to double values
	\**************************************************************************/
	for (int i=0; i<samples; i++)
		*work++ = (samples == 1) ? (*src8++) * scale : (*src16++) * scale;
	work = dmgr.asDouble(_work);

	/**************************************************************************\
	|* There are three cases:
	|* 1: The number of elems from before + this batch < fftSize
	\**************************************************************************/
	if (_previous.size() + samples < _fftSize*2)
		{
		for (int i=0; i<samples; i++)
			_previous.enqueue(work[i]);
		}
	else
		{
		while (samples > _fftSize*2)
			{
			/******************************************************************\
			|* 2: There are entries left over from the last run
			\******************************************************************/
			if (_previous.size() > 0)
				{
				TaskFFT  *task = new TaskFFT(_previous.data(),
											 _previous.size(),
											 work,
											 _fftSize * 2 - _previous.size());
				samples -= _previous.size();
				work += _fftSize * 2 - _previous.size();
				_previous.clear();
				QThreadPool::globalInstance()->start(task);
				}
			else
				{
				/**************************************************************\
				|* 3: We just have to iterate through the passed-in data
				\**************************************************************/
				TaskFFT *task = new TaskFFT(work, _fftSize*2);
				QThreadPool::globalInstance()->start(task);

				work += _fftSize*2;
				samples -= _fftSize*2;
				}
			}

		/**********************************************************************\
		|* If any samples are left over, enqueue them for the next pass
		\**********************************************************************/
		if (samples > 0)
			{
			for (int i=0; i<samples; i++)
				_previous.enqueue(*work ++);
			}
		}
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

	if (_fft >= 0)
		dmgr.release(_fft);
	_fft		= dmgr.blockFor(_fftSize, sizeof(std::complex<double>));

	if (_aggregate >= 0)
		dmgr.release(_aggregate);
	_aggregate	= dmgr.blockFor(_fftSize, sizeof(double));;

	if (_work >= 0)
		dmgr.release(_work);
	_work	= dmgr.blockFor(Config::instance().sampleRate(), sizeof(double));;
	}
