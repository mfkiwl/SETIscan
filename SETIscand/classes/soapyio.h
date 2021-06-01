#ifndef SOAPYIO_H
#define SOAPYIO_H

#include <string>
#include <vector>

#include <QObject>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

#include "properties.h"

class SoapyIO : public QObject
	{
	Q_OBJECT

		/**************************************************************************\
		|* typedefs and enums
		\**************************************************************************/
		typedef QMap<QString, QString>		RadioInfo;
		typedef std::vector<std::string>	StringList;
		typedef SoapySDR::RangeList			RangeList;

		/**************************************************************************\
		|* Properties
		\**************************************************************************/
		GET(int, channel);
		GET(SoapySDR::Device *, dev);
		GET(StringList, antennas);
		GET(SoapySDR::Range, gains);
		GET(RangeList, frequencyRanges);
		GET(RangeList, sampleRates);
		GET(RangeList, bandwidths);
		GET(QString, format);

	private:
		/**********************************************************************\
		|* Private variables
		\**********************************************************************/

		/**********************************************************************\
		|* Private methods
		\**********************************************************************/
		void _findMatchingRadio(void);
		void _getLists(void);
		bool _inRange(double value, RangeList &ranges);


	public:
		/**********************************************************************\
		|* Constructor
		\**********************************************************************/
		explicit SoapyIO(QObject *parent = nullptr);

		/**********************************************************************\
		|* Set the sample rate in Hz, with a bounds check
		\**********************************************************************/
		bool setSampleRate(int sampleRate);

		/**********************************************************************\
		|* Set the frequency in Hx, with a bounds check
		\**********************************************************************/
		bool setFrequency(int frequency);

		/**********************************************************************\
		|* Set the gain in dB, with a bounds check
		\**********************************************************************/
		bool setGain(double gain);



	};

#endif // SOAPYIO_H
