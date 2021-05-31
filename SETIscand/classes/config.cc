#include <QCoreApplication>
#include <QObject>
#include <QSettings>

#include "config.h"

/******************************************************************************\
|* These are the keys we look for in the config file
\******************************************************************************/
#define RADIO_GROUP			"radio"
#define DSP_GROUP			"dsp"

#define DRIVER_KEY			"filter-driver"
#define MODEL_KEY			"filter-model"
#define ID_KEY				"filter-id"
#define FREQUENCY_KEY		"frequency"
#define GAIN_KEY			"gain"

#define FFT_WINDOW_TYPE_KEY	"fft-window-type"


/******************************************************************************\
|* These are the commandline args we're managing
\******************************************************************************/
Q_GLOBAL_STATIC_WITH_ARGS(const QCommandLineOption,
		_help,
		({"h", "help"}, "Show this useful help"))
Q_GLOBAL_STATIC_WITH_ARGS(const QCommandLineOption,
		_driverFilter,
		(DRIVER_KEY, "Filter for the driver name", "sdrplay"))
Q_GLOBAL_STATIC_WITH_ARGS(const QCommandLineOption,
		_modeFilter,
		(MODEL_KEY, "Filter for the mode name", ""))
Q_GLOBAL_STATIC_WITH_ARGS(const QCommandLineOption,
		_idFilter,
		(ID_KEY, "Filter for the device-id", ""))
Q_GLOBAL_STATIC_WITH_ARGS(const QCommandLineOption,
		_frequency,
		({"f", "frequency"}, "Center-frequencty to tune to", "1420406000"))
Q_GLOBAL_STATIC_WITH_ARGS(const QCommandLineOption,
		_fftWindow,
		({"w", "fft-window-type"}, "Window-type for FFT", "hamming"))
Q_GLOBAL_STATIC_WITH_ARGS(const QCommandLineOption,
		_gain,
		({"g", "gain"}, "Gain to apply"))
Q_GLOBAL_STATIC_WITH_ARGS(const QCommandLineOption,
		_version,
		({"v", "version"}, "Display the program version"))

Q_GLOBAL_STATIC_WITH_ARGS(const QCommandLineOption,
		_listAntennas,
		("list-antennas", "List antennas and exit"))
Q_GLOBAL_STATIC_WITH_ARGS(const QCommandLineOption,
		_listGains,
		("list-gains", "List gains and exit"))
Q_GLOBAL_STATIC_WITH_ARGS(const QCommandLineOption,
		_listFrequencies,
		("list-frequencies", "List frequency-ranges and exit"))

/******************************************************************************\
|* Read configuration from both commandline and settings
\******************************************************************************/
Config::Config()
	{
	_parser.setApplicationDescription("Seti scanning daemon");
	_parser.addOption(*_driverFilter);
	_parser.addOption(*_idFilter);
	_parser.addOption(*_frequency);
	_parser.addOption(*_gain);
	_parser.addOption(*_help);
	_parser.addOption(*_listAntennas);
	_parser.addOption(*_listFrequencies);
	_parser.addOption(*_listGains);
	_parser.addOption(*_modeFilter);
	_parser.addOption(*_version);
	_parser.addOption(*_fftWindow);

	_parser.parse(QCoreApplication::arguments());

	if (_parser.isSet(*_help))
		{
		QString help = _parser.helpText();
		fprintf(stderr, "%s\n\n"
			"Window types for the FFT can be (use name or number):\n"
			" 0: Rectangle     1: Hamming      2: Hanning\n"
			" 3: Blackman      4: Welch        5: Parzen\n\n"
			,qUtf8Printable(help)
			);
		exit(0);
		}


	if (_parser.isSet(*_version))
		_parser.showVersion();
	}

/******************************************************************************\
|* Get the driver filter
\******************************************************************************/
QString Config::radioDriverFilter(void)
	{
	if (_parser.isSet(*_driverFilter))
		return _parser.value(*_driverFilter).toLower();

	QSettings s;
	s.beginGroup(RADIO_GROUP);
	QString filter = s.value(DRIVER_KEY, "sdrplay").toString().toLower();
	s.endGroup();
	return filter;
	}


/******************************************************************************\
|* Get the mode filter
\******************************************************************************/
QString Config::radioModeFilter(void)
	{
	if (_parser.isSet(*_modeFilter))
		return _parser.value(*_modeFilter).toLower();

	QSettings s;
	s.beginGroup(RADIO_GROUP);
	QString filter = s.value(MODEL_KEY, "").toString().toLower();
	s.endGroup();
	return filter;
	}

/******************************************************************************\
|* Get the id filter
\******************************************************************************/
int Config::radioIdFilter(void)
	{
	if (_parser.isSet(*_idFilter))
		return _parser.value(*_idFilter).toInt();

	QSettings s;
	s.beginGroup(RADIO_GROUP);
	QString filter = s.value(ID_KEY, "-1").toString();
	s.endGroup();
	return filter.toInt();
	}

/******************************************************************************\
|* Get the frequency to tune to
\******************************************************************************/
int Config::centerFrequency(void)
	{
	if (_parser.isSet(*_frequency))
		return _parser.value(*_frequency).toInt();

	QSettings s;
	s.beginGroup(RADIO_GROUP);
	QString freq = s.value(FREQUENCY_KEY, "1420406000").toString();
	s.endGroup();
	return freq.toInt();
	}

/******************************************************************************\
|* Get the gain to apply
\******************************************************************************/
int Config::gain(void)
	{
	if (_parser.isSet(*_gain))
		return _parser.value(*_gain).toInt();

	QSettings s;
	s.beginGroup(RADIO_GROUP);
	QString gain = s.value(GAIN_KEY, "40").toString();
	s.endGroup();
	return gain.toInt();
	}


/******************************************************************************\
|* Get the fft-windowing function
\******************************************************************************/
Config::WindowType Config::fftWindowType(void)
	{
	QString window = "";
	if (_parser.isSet(*_fftWindow))
		window = _parser.value(*_fftWindow);
	else
		{
		QSettings s;
		s.beginGroup(DSP_GROUP);
		window = s.value(FFT_WINDOW_TYPE_KEY, "hamming").toString();
		s.endGroup();
		}

	QMap<QString,Config::WindowType> map =
		{
			{"0", Config::W_RECTANGLE},
			{"1", Config::W_HAMMING},
			{"2", Config::W_HANNING},
			{"3", Config::W_BLACKMAN},
			{"4", Config::W_WELCH},
			{"5", Config::W_PARZEN},
			{"rectangle", Config::W_RECTANGLE},
			{"hamming", Config::W_HAMMING},
			{"hanning", Config::W_HANNING},
			{"blackman", Config::W_BLACKMAN},
			{"welch", Config::W_WELCH},
			{"parzen", Config::W_PARZEN},
		};

	QString key = window.toLower();
	if (map.contains(key))
		return map[key];

	qWarning() << "Cannot find window function for " << key << " - using Hamming";
	return Config::W_HAMMING;
	}

/******************************************************************************\
|* Get whether to list out the antennas
\******************************************************************************/
bool Config::listAntennas(void)
	{
	return _parser.isSet(*_listAntennas);
	}

/******************************************************************************\
|* Get whether to list out the antennas
\******************************************************************************/
bool Config::listGains(void)
	{
	return _parser.isSet(*_listGains);
	}

/******************************************************************************\
|* Get whether to list out the antennas
\******************************************************************************/
bool Config::listFrequencyRanges(void)
	{
	return _parser.isSet(*_listFrequencies);
	}

