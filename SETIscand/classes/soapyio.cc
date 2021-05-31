#include "config.h"
#include "constants.h"
#include "soapyio.h"

#include <QLocale>
#include <SoapySDR/Logger.hpp>

#define DRIVER_KEY			"driver"
#define MODE_KEY			"mode"

/******************************************************************************\
|* Categorised logging support
\******************************************************************************/
Q_LOGGING_CATEGORY(log_dsp, "seti.dsp   ")

#define LOG qDebug(log_dsp) << QTime::currentTime().toString("hh:mm:ss.zzz")
#define ERR qCritical(log_dsp) << QTime::currentTime().toString("hh:mm:ss.zzz")

/******************************************************************************\
|* Constructor
\******************************************************************************/
SoapyIO::SoapyIO(QObject *parent)
		:QObject(parent)
		,_dev(nullptr)
	{
	SoapySDR::setLogLevel(SOAPY_SDR_CRITICAL);

	_findMatchingRadio();
	if (_dev != nullptr)
		{
		_getLists();
		}
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
	_antennas			= _dev->listAntennas(SOAPY_SDR_RX, 0);
	bool shouldExit		= false;

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
		fflush(stdout);
		shouldExit = true;
		}

	_gains = _dev->getGainRange(SOAPY_SDR_RX, 0);
	if (Config::instance().listGains())
		{
		QLocale l = QLocale::system();
		printf("Gains: {%s -> %s Db}\n",
			   qPrintable(l.toString((int)_gains.minimum())),
			   qPrintable(l.toString((int)_gains.maximum())));
		fflush(stdout);
		shouldExit = true;
		}

	_frequencyRanges	= _dev->getFrequencyRange(SOAPY_SDR_RX, 0);
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
		fflush(stdout);
		shouldExit = true;
		}

	if (shouldExit)
		::exit(0);
	}
