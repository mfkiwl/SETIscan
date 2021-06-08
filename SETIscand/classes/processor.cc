#include <complex>

#include <QThreadPool>

#include "config.h"
#include "constants.h"
#include "datamgr.h"
#include "fftaggregator.h"
#include "processor.h"
#include "soapyio.h"
#include "taskfft.h"

/******************************************************************************\
|* Categorised logging support
\******************************************************************************/
#define LOG qDebug(log_dsp) << QTime::currentTime().toString("hh:mm:ss.zzz")
#define ERR qCritical(log_dsp) << QTime::currentTime().toString("hh:mm:ss.zzz")

/******************************************************************************\
|* Constructor
\******************************************************************************/
Processor::Processor(Config& cfg, QObject *parent)
		  : QObject(parent)
		  ,_cfg(cfg)
		  ,_sio(nullptr)
		  ,_fftSize(0)
		  ,_work(-1)
		  ,_fftIn(-1)
		  ,_fftOut(-1)
	{
	_aggregator = new FFTAggregator(this);
	_aggregator->moveToThread(&_bgThread);
	_bgThread.start();
	}

/******************************************************************************\
|* Destructor
\******************************************************************************/
Processor::~Processor(void)
	{
	DataMgr &dmgr	= DataMgr::instance();
	ERR << "Destroying processor";

	if (_fftIn >= 0)
		dmgr.release(_fftIn);
	if (_fftOut >= 0)
		dmgr.release(_fftOut);
	if (_work >= 0)
		dmgr.release(_work);
	}

/******************************************************************************\
|* We got data back
\******************************************************************************/
void Processor::dataReceived(int64_t buffer, int samples, int max, int bytes)
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
		*work++ = (bytes == 1) ? (*src8++) * scale : (*src16++) * scale;
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
			TaskFFT *task = nullptr;

			/******************************************************************\
			|* 2: There are entries left over from the last run
			\******************************************************************/
			if (_previous.size() > 0)
				{
				task = new TaskFFT(_previous.data(),
								   _previous.size(),
								   work,
								   _fftSize * 2 - _previous.size());

				samples -= _previous.size();
				work += _fftSize * 2 - _previous.size();
				_previous.clear();
				}
			else
				{
				/**************************************************************\
				|* 3: We just have to iterate through the passed-in data
				\**************************************************************/
				task = new TaskFFT(work, _fftSize*2);

				work += _fftSize*2;
				samples -= _fftSize*2;
				}

			connect(task, &TaskFFT::fftDone,
					_aggregator, &FFTAggregator::fftReady);

			task->setPlan(_fftPlan);
			QThreadPool::globalInstance()->start(task);

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

	/**************************************************************************\
	|* Create the FFT plan. We won't actually use these buffers, but we can
	|* substitute others as long as they are compatible, so allocate these
	|* in exactly the same way as the ones we will use.
	\**************************************************************************/
	LOG << "Creating FFT plan";
	DataMgr &dmgr		= DataMgr::instance();
	fftw_complex *in	= dmgr.asFFT(_fftIn);
	fftw_complex *out	= dmgr.asFFT(_fftOut);
	_fftPlan			= fftw_plan_dft_1d(_fftSize,
										   in,
										   out,
										   FFTW_FORWARD,
										   FFTW_PATIENT);
	LOG << "FFT plan created";
	}

/******************************************************************************\
|* Set up the buffers
\******************************************************************************/
void Processor::_allocate(void)
	{
	DataMgr &dmgr = DataMgr::instance();

	if (_work >= 0)
		dmgr.release(_work);
	_work	= dmgr.blockFor(Config::instance().sampleRate(), sizeof(double));

	if (_fftIn >= 0)
		dmgr.release(_fftIn);
	_fftIn	= dmgr.fftBlockFor(_fftSize);

	if (_fftOut >= 0)
		dmgr.release(_fftOut);
	_fftOut	= dmgr.fftBlockFor(_fftSize);
	}
