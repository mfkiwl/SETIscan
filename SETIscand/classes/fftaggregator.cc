#include <QDateTime>

#include "config.h"
#include "constants.h"
#include "datamgr.h"
#include "fftaggregator.h"

/******************************************************************************\
|* Categorised logging support
\******************************************************************************/
#define LOG  qDebug(log_dsp) << QTime::currentTime().toString("hh:mm:ss.zzz")
#define WARN qWarning(log_dsp) << QTime::currentTime().toString("hh:mm:ss.zzz")
#define ERR	 qCritical(log_dsp) << QTime::currentTime().toString("hh:mm:ss.zzz")

/******************************************************************************\
|* Constructor
\******************************************************************************/
FFTAggregator::FFTAggregator(QObject *parent)
			  :QObject(parent)
			  ,_fftSize(0)
			  ,_haveData(false)
			  ,_updateSecs(5)
			  ,_sampleSecs(300)
			  ,_nextUpdate(0)
			  ,_nextSample(0)
			  ,_updatePasses(0)
			  ,_samplePasses(0)
			  ,_updateData(nullptr)
			  ,_sampleData(nullptr)

	{
	Config &cfg = Config::instance();
	_fftSize	= cfg.fftSize();
	_updateSecs	= cfg.secondsBetweenUpdates();
	_sampleSecs	= cfg.secondsBetweenSamples();

	_updateData	= new double[_fftSize];
	memset(_updateData, 0, _fftSize * sizeof(double));

	_sampleData	= new double[_fftSize];
	memset(_sampleData, 0, _fftSize * sizeof(double));
	}

/******************************************************************************\
|* Destructor
\******************************************************************************/
FFTAggregator::~FFTAggregator(void)
	{
	if (_updateData != nullptr)
		delete [] _updateData;
	if (_sampleData != nullptr)
		delete [] _sampleData;
	}

/******************************************************************************\
|* We've been sent an FFT packet. Aggregate it
\******************************************************************************/
void FFTAggregator::fftReady(int buffer)
	{
	QMutexLocker guard(&_lock);
	DataMgr &dmgr	= DataMgr::instance();

	/**************************************************************************\
	|* Set up the next sample/update point if we haven't got one. That way we
	|* wait until data is streaming in before we start counting
	\**************************************************************************/
	if (_haveData == false)
		{
		_haveData		= true;
		_nextUpdate		= _deltaT(_updateSecs);
		_nextSample		= _deltaT(_sampleSecs);
		_updatePasses	= 0;
		_samplePasses	= 0;
		}

	/**************************************************************************\
	|* aggregate this pass
	\**************************************************************************/
	fftw_complex* data  = dmgr.asFFT(buffer);
	for (int i=0; i<_fftSize; i++)
		{
		double creal	= data[i][0] * data[i][0];
		double cimag	= data[i][1] * data[i][1];
		double power	= creal * creal + cimag * cimag;
		double mag		= 0.05 * log(power+1);

		// FIXME: Add normalisation here
		// mag -= _normalisation[i];

		_updateData[i] += mag;
		_updatePasses  ++;

		_sampleData[i] += mag;
		_samplePasses  ++;
		}

	/**************************************************************************\
	|* Check whether we're past the time for an update
	\**************************************************************************/
	if (QDateTime::currentMSecsSinceEpoch() >= _nextUpdate)
		{
		// Create copy of buffer and send to update thread
		for (int i=0; i<_fftSize; i++)
			_updateData[i] /= _updatePasses;

		memset(_updateData, 0, _fftSize * sizeof(double));
		_nextUpdate		= _deltaT(_updateSecs);
		_updatePasses	= 0;

		dmgr.retain(buffer);
		emit aggregatedDataReady(TYPE_UPDATE, buffer);
		}

	/**************************************************************************\
	|* Check whether we're past the time for a sample
	\**************************************************************************/
	if (QDateTime::currentMSecsSinceEpoch() >= _nextSample)
		{
		// Create copy of buffer and send to update thread
		for (int i=0; i<_fftSize; i++)
			_sampleData[i] /= _samplePasses;

		memset(_sampleData, 0, _fftSize * sizeof(double));
		_nextSample		= _deltaT(_sampleSecs);
		_samplePasses	= 0;

		dmgr.retain(buffer);
		emit aggregatedDataReady(TYPE_SAMPLE, buffer);
		}

	dmgr.release(buffer);
	}

/*****************************************************************************\
|* Produce a delta-time relative to now
\******************************************************************************/
qint64 FFTAggregator::_deltaT(double delta)
	{
	qint64 now	 = QDateTime::currentMSecsSinceEpoch();
	int secs	= (int)delta;
	int msecs	= (int)(1000 * (delta - secs));

	now += secs * 1000;
	now += msecs;
	return now;
	}
