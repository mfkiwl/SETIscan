#ifndef SOAPYIO_H
#define SOAPYIO_H

#include <QObject>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

class SoapyIO : public QObject
	{
	Q_OBJECT
	/**************************************************************************\
	|* Properties
	\**************************************************************************/

	public:
		explicit SoapyIO(QObject *parent = nullptr);


	};

#endif // SOAPYIO_H
