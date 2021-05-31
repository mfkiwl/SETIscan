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
		GET(SoapySDR::Device *, dev);
		GET(StringList, antennas);
		GET(SoapySDR::Range, gains);
		GET(RangeList, frequencyRanges);

	private:
		/**********************************************************************\
		|* Private variables
		\**********************************************************************/

		/**********************************************************************\
		|* Private methods
		\**********************************************************************/
		void _findMatchingRadio(void);
		void _getLists(void);


	public:
		/**********************************************************************\
		|* Constructor
		\**********************************************************************/
		explicit SoapyIO(QObject *parent = nullptr);



	};

#endif // SOAPYIO_H
