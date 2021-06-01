#ifndef CONFIG_H
#define CONFIG_H

#include <QCommandLineParser>

#include "singleton.h"

class Config : public Singleton<Config>
	{
	private:
		QCommandLineParser		_parser;

	public:
		/******************************************************************\
		|* Typedefs and enums
		\******************************************************************/
		typedef enum
			{
			W_RECTANGLE	= 0,
			W_HAMMING,
			W_HANNING,
			W_BLACKMAN,
			W_WELCH,
			W_PARZEN
			} WindowType;

		/**********************************************************************\
		|* Constructor
		\**********************************************************************/
		explicit Config();

		/******************************************************************\
		|* Return the FFT window type
		\******************************************************************/
		WindowType fftWindowType(void);

		/******************************************************************\
		|* Return the frequency to tune to
		\******************************************************************/
		int centerFrequency(void);

		/******************************************************************\
		|* Return the gain to apply
		\******************************************************************/
		double gain(void);

		/******************************************************************\
		|* Return the baseband sample rate to use
		\******************************************************************/
		int sampleRate(void);

		/******************************************************************\
		|* Return whether to list out criteria. These are only on the
		|* commandline
		\******************************************************************/
		bool listAntennas(void);
		bool listGains(void);
		bool listFrequencyRanges(void);
		bool listSampleRates(void);
		bool listBandwidths(void);
		bool listNativeFormat(void);

		/******************************************************************\
		|* Return the filter on the driver to select out the one we want
		\******************************************************************/
		QString radioDriverFilter(void);

		/******************************************************************\
		|* Return the filter on the mode to select out the one we want
		\******************************************************************/
		QString radioModeFilter(void);

		/******************************************************************\
		|* Return the filter on the device-id to select out the one we want
		\******************************************************************/
		int radioIdFilter(void);

	};

#endif // CONFIG_H