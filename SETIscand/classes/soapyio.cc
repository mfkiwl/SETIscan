#include "config.h"
#include "constants.h"
#include "processor.h"
#include "soapyio.h"
#include "soapyworker.h"

#include <QLocale>
#include <QString>
#include <SoapySDR/Logger.hpp>
#include <SoapySDR/Errors.hpp>

#define DRIVER_KEY			"driver"
#define MODE_KEY			"mode"

/******************************************************************************\
|* Categorised logging support
\******************************************************************************/
Q_LOGGING_CATEGORY(log_dsp, "seti.dsp   ")

#define LOG  qDebug(log_dsp) << QTime::currentTime().toString("hh:mm:ss.zzz")
#define WARN qWarning(log_dsp) << QTime::currentTime().toString("hh:mm:ss.zzz")
#define ERR	 qCritical(log_dsp) << QTime::currentTime().toString("hh:mm:ss.zzz")

/******************************************************************************\
|* Constructor
\******************************************************************************/
SoapyIO::SoapyIO(Processor *processor)
		:QObject(processor)
		,_channel(0)
		,_dev(nullptr)
		,_sampleRate(0)
		,_thread(nullptr)
		,_worker(nullptr)
		,_rx(nullptr)
		,_proc(processor)
	{
	//SoapySDR::setLogLevel(SOAPY_SDR_CRITICAL);

	_findMatchingRadio();
	if (_dev != nullptr)
		{
		_getLists();

		setSampleRate(Config::instance().sampleRate());
		setFrequency(Config::instance().centerFrequency());
		setGain(Config::instance().gain());
		setAntenna(Config::instance().antenna());
		}
	}

/******************************************************************************\
|* Destructor
\******************************************************************************/
SoapyIO::~SoapyIO(void)
	{
	if (_thread != nullptr)
		stopWorker();
	if (_rx)
		{
		_dev->deactivateStream(_rx, 0, 0);
		_dev->closeStream(_rx);
		}
	if (_dev)
		SoapySDR::Device::unmake(_dev);
	}

/******************************************************************************\
|* Set the sample rate with a bounds check
\******************************************************************************/
bool SoapyIO::setSampleRate(int sampleRate)
	{
	QLocale l = QLocale::system();
	bool ok = _inRange(sampleRate, _sampleRates);
	if (ok)
		{
		_dev->setSampleRate(SOAPY_SDR_RX, _channel, sampleRate);
		LOG << "Set sample rate to" << l.toString(sampleRate);

		_sampleRate = (int)(_dev->getSampleRate(SOAPY_SDR_RX, _channel));
		if (_sampleRate != sampleRate)
			{
			QString msg = QString("Real sample rate (%1) differs from requested (%2)")
					.arg(l.toString(_sampleRate), l.toString(sampleRate));
			WARN << msg;
			}
		}
	else
		WARN << "Sample rate " << sampleRate << "is outside of allowed ranges";
	return ok;
	}

/******************************************************************************\
|* Set the sample rate with a bounds check
\******************************************************************************/
bool SoapyIO::setFrequency(int frequency)
	{
	QLocale l = QLocale::system();

	bool ok = _inRange(frequency, _frequencyRanges);
	if (ok)
		{
		_dev->setFrequency(SOAPY_SDR_RX, _channel, frequency);
		LOG << "Set frequency to" << l.toString(frequency);

		int realFreq = (int)(_dev->getFrequency(SOAPY_SDR_RX, _channel));
		if (realFreq != frequency)
			{
			QString msg = QString("Real frequency (%1) differs from requested (%2)")
					.arg(l.toString(realFreq), l.toString(frequency));
			WARN << msg;
			}
		}
	else
		WARN << "Frequency " << frequency << "is outside of allowed ranges";
	return ok;
	}

/******************************************************************************\
|* Set the antenna using name or index
\******************************************************************************/
bool SoapyIO::setAntenna(QString nameOrIndex)
	{
	std::vector<std::string> list = _dev->listAntennas(SOAPY_SDR_RX, _channel);

	/**************************************************************************\
	|* Conver to a name if an index is given
	\**************************************************************************/
	bool isNumeric	= false;
	size_t index	= nameOrIndex.toInt(&isNumeric);
	if (index < 0)
		{
		ERR << "Cannot use  negative index to locate antennas";
		return false;
		}

	if (isNumeric)
		{
		if (index < list.size())
			nameOrIndex = list[index].c_str();
		else
			{
			ERR << "Antenna index out of range, max=" << list.size()-1;
			return false;
			}
		}
	else
		{
		std::string pattern = nameOrIndex.toStdString();
		for (std::string& name : list)
			if (name.find(pattern) >= 0)
				{
				nameOrIndex = name.c_str();
				break;
				}
		}

	_dev->setAntenna(SOAPY_SDR_RX, _channel, nameOrIndex.toStdString());
	LOG << "Set antenna to" << nameOrIndex;

	return true;
	}


/******************************************************************************\
|* Set the sample rate with a bounds check
\******************************************************************************/
bool SoapyIO::setGain(double gain)
	{
	QLocale l = QLocale::system();

	bool ok = ((gain >= _gains.minimum()) && (gain <= _gains.maximum()));
	if (ok)
		{
		_dev->setGain(SOAPY_SDR_RX, _channel, gain);
		LOG << "Set gain to" << l.toString(gain);

		int realGain = (_dev->getGain(SOAPY_SDR_RX, _channel));
		if (realGain != gain)
			{
			QString msg = QString("Real gain (%1) differs from requested (%2)")
					.arg(l.toString(realGain), l.toString(gain));
			WARN << msg;
			}
		}
	else
		WARN << "Gain " << gain << "is outside of allowed range";

	return ok;
	}


/******************************************************************************\
|* Start a worker going in the background, creating it if necessary
\******************************************************************************/
void SoapyIO::startWorker(void)
	{
	if (_worker == nullptr)
		{
		_thread = new QThread(this);
		_worker = new SoapyWorker();
		_worker->setSdr(this);

		_worker->moveToThread(_thread);

		connect(this, &SoapyIO::startWorkerSampling,
				_worker, &SoapyWorker::startSampling);
		connect(this, &SoapyIO::stopWorkerSampling,
				_worker, &SoapyWorker::stopSampling);
		connect(_thread, &QThread::finished,
				_worker, &QObject::deleteLater);
		connect(_worker, &SoapyWorker::dataAvailable,
				_proc, &Processor::dataReceived);
		_thread->start();

		emit startWorkerSampling();
		}
	}

/******************************************************************************\
|* Kill off the background thread
\******************************************************************************/
void SoapyIO::stopWorker(void)
	{
	if (_worker != nullptr)
		{
		emit stopWorkerSampling();
		_thread->quit();
		_thread->wait();
		}
	}

/******************************************************************************\
|* Determine the characteristics of a stream-description
\******************************************************************************/
bool SoapyIO::isComplexStream(void)
	{
	if (_format.at(0) == QChar('C'))
		return true;
	return false;
	}

bool SoapyIO::isFloatStream(void)
	{
	int idx = 0;
	if (isComplexStream())
		idx ++;
	if (_format.at(idx) == QChar('F'))
		return true;
	return false;
	}

bool SoapyIO::isSignedStream(void)
	{
	int idx = 0;
	if (isComplexStream())
		idx ++;
	if (_format.at(idx) == QChar('S'))
		return true;
	return false;
	}

bool SoapyIO::isUnsignedStream(void)
	{
	int idx = 0;
	if (isComplexStream())
		idx ++;
	if (_format.at(idx) == QChar('U'))
		return true;
	return false;
	}

int SoapyIO::sampleBytes(void)
	{
	int isComplex = isComplexStream();
	int idx = (isComplex) ? 2 : 1;
	int bits = _format.mid(idx).toInt();
	bits = (isComplex) ? bits * 2 : bits;
	return bits/8;
	}


/******************************************************************************\
|* Create or return the current RX stream
\******************************************************************************/
SoapySDR::Stream * SoapyIO::rxStream(void)
	{
	if (_rx == nullptr)
		{
		std::string fmt					= _format.toStdString();
		std::vector<size_t>  channels	= {(size_t)_channel};
		SoapySDR::Kwargs args;
		args["WIRE"] = fmt;

		_rx				= _dev->setupStream(SOAPY_SDR_RX, fmt, channels, args);
		int error		= _dev->activateStream(_rx, 0, 0);
		if (error != 0)
			ERR << "Got error" << SoapySDR::errToStr(error) << "for" << fmt.c_str();
		}
	return _rx;
	}

/******************************************************************************\
|* Read data from a stream
\******************************************************************************/
int SoapyIO::waitForData(SoapySDR::Stream *stream,
						 void *const *buffers,
						 int elems,
						 int &flags,
						 long long ns,
						 const long timeoutUs)
	{
	return _dev->readStream(stream, buffers, elems, flags, ns, timeoutUs);
	}


/******************************************************************************\
|* Determine the radio to use by probing and filtering the results
\******************************************************************************/
void SoapyIO::_findMatchingRadio(void)
	{
	QString devFilter	= Config::instance().radioDriverFilter();
	QString modeFilter	= Config::instance().radioModeFilter();
	int idFilter		= Config::instance().radioIdFilter();

	SoapySDR::KwargsList results = SoapySDR::Device::enumerate();
	SoapySDR::Kwargs::iterator it;

	for(size_t i = 0; i < results.size(); ++i)
		{
		RadioInfo info;
		for( it = results[i].begin(); it != results[i].end(); ++it)
			{
			QString key = it->first.c_str();
			QString val = it->second.c_str();
			info[key.toLower()] = val.toLower();
			}

		bool driverFound	= true;
		bool modeFound		= true;
		bool devFound		= true;

		if (devFilter.length() > 0)
			if (info.contains(DRIVER_KEY))
				{
				QString driver = info[DRIVER_KEY];
				if (!driver.contains(devFilter))
					driverFound = false;
				}
		if (modeFilter.length() > 0)
			if (info.contains(MODE_KEY))
				if (!info[MODE_KEY].contains(modeFilter))
					modeFound = false;

		if (idFilter >= 0)
			if ((int)i != idFilter)
				devFound = false;

		bool found = driverFound & modeFound & devFound;
		if (found)
			{
			_dev = SoapySDR::Device::make(results[i]);
			break;
			}
		}

	if (_dev == nullptr)
		{
		QString msg = QString("Cannot match device using {driver:%1, mode:%2, id:%3}").
				arg(devFilter,modeFilter).arg(idFilter);
		ERR << msg;
		}
	}


/******************************************************************************\
|* Get the list of antennas this SDR supports
\******************************************************************************/
void SoapyIO::_getLists(void)
	{
	_antennas			= _dev->listAntennas(SOAPY_SDR_RX, _channel);
	bool shouldExit		= false;

	/**************************************************************************\
	|* Antennas
	\**************************************************************************/
	if (Config::instance().listAntennas())
		{
		printf("Antennas");
		char comma = ':';
		for (std::string &antenna : _antennas)
			{
			printf("%c %s", comma, antenna.c_str());
			comma = ',';
			}
		printf("\n");
		shouldExit = true;
		}

	/**************************************************************************\
	|* Gain range - this means all the gains in the device
	\**************************************************************************/
	_gains = _dev->getGainRange(SOAPY_SDR_RX, _channel);
	if (Config::instance().listGains())
		{
		QLocale l = QLocale::system();
		printf("Gains: {%s -> %s Db}\n",
			   qPrintable(l.toString((int)_gains.minimum())),
			   qPrintable(l.toString((int)_gains.maximum())));
		shouldExit = true;
		}

	/**************************************************************************\
	|* Frequency ranges
	\**************************************************************************/
	_frequencyRanges	= _dev->getFrequencyRange(SOAPY_SDR_RX, _channel);
	if (Config::instance().listFrequencyRanges())
		{
		QLocale l = QLocale::system();

		printf("Frequencies");
		char comma = ':';
		for (SoapySDR::Range &range : _frequencyRanges)
			{
			printf("%c {%s -> %s Hz}", comma,
				   qPrintable(l.toString((int)range.minimum())),
				   qPrintable(l.toString((int)range.maximum())));
			comma = ',';
			}
		printf("\n");
		shouldExit = true;
		}

	/**************************************************************************\
	|* Sample rates
	\**************************************************************************/
	_sampleRates = _dev->getSampleRateRange(SOAPY_SDR_RX, _channel);
	if (Config::instance().listSampleRates())
		{
		QLocale l = QLocale::system();

		printf("Sample rates");
		char comma = ':';
		for (SoapySDR::Range &range : _sampleRates)
			{
			printf("%c {%s -> %s Hz}", comma,
				   qPrintable(l.toString((int)range.minimum())),
				   qPrintable(l.toString((int)range.maximum())));
			comma = ',';
			}
		printf("\n");
		shouldExit = true;
		}

	/**************************************************************************\
	|* Baseband bandwidths
	\**************************************************************************/
	_bandwidths = _dev->getBandwidthRange(SOAPY_SDR_RX, _channel);
	if (Config::instance().listBandwidths())
		{
		QLocale l = QLocale::system();

		printf("Bandwidths");
		char comma = ':';
		for (SoapySDR::Range &range : _bandwidths)
			{
			printf("%c {%s -> %s Hz}", comma,
				   qPrintable(l.toString((int)range.minimum())),
				   qPrintable(l.toString((int)range.maximum())));
			comma = ',';
			}
		printf("\n");
		shouldExit = true;
		}


	/**************************************************************************\
	|* Stream format
	\**************************************************************************/
	double max;
	_format		= _dev->getNativeStreamFormat(SOAPY_SDR_RX, 0, max).c_str();
	_maxValue	= (int)max;

	if (Config::instance().listNativeFormat())
		{
		printf("Native streaming format: %s [max:%d]\n",
				qPrintable(_format), _maxValue);
		shouldExit = true;
		}


	/**************************************************************************\
	|* Channels
	\**************************************************************************/
	if (Config::instance().listChannels())
		{
		int numChannels = _dev->getNumChannels(SOAPY_SDR_RX);
		for (int i=0; i<numChannels; i++)
			{
			auto args	= _dev->getChannelInfo(SOAPY_SDR_RX, i);

			printf("Channel %d {%d properties}\n", i, (int)args.size());
			auto it		= args.begin();

			while (it != args.end())
				{
				std::string key = it->first;
				std::string val = it->second;
				printf("   %s : %s\n", key.c_str(), val.c_str());
				}
			}
		shouldExit = true;
		}

	fflush(stdout);
	if (shouldExit)
		::exit(0);
	}

/******************************************************************************\
|* Figure out if a value lies within a range-set
\******************************************************************************/
bool SoapyIO::_inRange(double value, RangeList &ranges)
	{
	for (SoapySDR::Range& range : ranges)
		if ((value >= range.minimum()) && (value <= range.maximum()))
			return true;

	return false;
	}
