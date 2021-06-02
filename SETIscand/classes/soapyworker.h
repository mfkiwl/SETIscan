#ifndef SOAPYWORKER_H
#define SOAPYWORKER_H

#include <QObject>

#include "properties.h"
QT_FORWARD_DECLARE_CLASS(SoapyIO)

class SoapyWorker : public QObject
	{
	Q_OBJECT

	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GETSET(SoapyIO*, sdr, Sdr);

	/**************************************************************************\
	|* Public methods
	\**************************************************************************/
	public:
		explicit SoapyWorker(QObject *parent = nullptr);

	/**************************************************************************\
	|* Public slots
	\**************************************************************************/
	public slots:
		void startSampling(void);
		void stopSampling(void);

	};

#endif // SOAPYWORKER_H
