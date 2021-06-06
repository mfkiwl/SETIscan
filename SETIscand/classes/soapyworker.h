#ifndef SOAPYWORKER_H
#define SOAPYWORKER_H

#include <QObject>
#include <SoapySDR/Device.hpp>

#include "properties.h"
QT_FORWARD_DECLARE_CLASS(SoapyIO)

class SoapyWorker : public QObject
	{
	Q_OBJECT

	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GETSET(SoapyIO*, sdr, Sdr);
	GET(bool, isActive);

	private:

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

	signals:
		void dataAvailable(int64_t handle, int elems, int max, int bytes);
	};

#endif // SOAPYWORKER_H
