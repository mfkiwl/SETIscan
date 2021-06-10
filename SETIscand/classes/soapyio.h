#ifndef SOAPYIO_H
#define SOAPYIO_H

#include <string>
#include <vector>

#include <QObject>
#include <QThread>

#include <SoapySDR/Device.hpp>
#include <SoapySDR/Types.hpp>
#include <SoapySDR/Formats.hpp>

#include "properties.h"

QT_FORWARD_DECLARE_CLASS(Processor)
QT_FORWARD_DECLARE_CLASS(SoapyWorker)

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
	GET(int, sampleRate);
	GET(RangeList, bandwidths);
	GET(QString, format);
	GET(int, maxValue);

	private:
		/**********************************************************************\
		|* Private variables
		\**********************************************************************/
		QThread *			_thread;			// Thread to tidy up later
		SoapyWorker *		_worker;			// Worker or nullptr
		SoapySDR::Stream *	_rx;				// Receiving-data stream
		Processor *			_proc;				// Processing chain

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
		explicit SoapyIO(Processor *processor);
		~SoapyIO(void);

		/**********************************************************************\
		|* Set the sample rate in Hz, with a bounds check
		\**********************************************************************/
		bool setSampleRate(int sampleRate);

		/**********************************************************************\
		|* Set the frequency in Hx, with a bounds check
		\**********************************************************************/
		bool setFrequency(int frequency);

		/**********************************************************************\
		|* Set the antenna using name or index
		\**********************************************************************/
		bool setAntenna(QString nameOrIndex);

		/**********************************************************************\
		|* Set the gain in dB, with a bounds check
		\**********************************************************************/
		bool setGain(double gain);

		/**********************************************************************\
		|* Start/Stop a worker sampling, creating it if necessary
		\**********************************************************************/
		void startWorker(void);
		void stopWorker(void);

		/**********************************************************************\
		|* Return a set-up stream
		\**********************************************************************/
		SoapySDR::Stream * rxStream(void);

		/**********************************************************************\
		|* Shim around the readStream call
		\**********************************************************************/
		int waitForData(SoapySDR::Stream *stream,
						void * const *buffers,
						int elems,
						int &flags,
						long long ns,
						const long timeoutUs=100000);

		/**********************************************************************\
		|* Stream characteristics
		\**********************************************************************/
		bool isComplexStream(void);
		bool isFloatStream(void);
		bool isUnsignedStream(void);
		bool isSignedStream(void);
		int sampleBytes(void);

	signals:
		/**********************************************************************\
		|* Start/Stop a worker sampling
		\**********************************************************************/
		void startWorkerSampling(void);
		void stopWorkerSampling(void);

	};

#endif // SOAPYIO_H
