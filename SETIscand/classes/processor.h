#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <QObject>
#include <QQueue>

#include "properties.h"

QT_FORWARD_DECLARE_CLASS(Config)
QT_FORWARD_DECLARE_CLASS(SoapyIO)

class Processor : public QObject
	{
	Q_OBJECT

	private:
		/**********************************************************************\
		|* Private variables
		\**********************************************************************/
		Config&			_cfg;			// Configuration
		SoapyIO *		_sio;			// IO object
		int				_fftSize;		// Size of the FFT

		int				_aggregate;		// Buffer to aggregate data into
		int				_fft;			// Buffer for FFT data
		int				_work;			// Working buffer
		QQueue<double>	_previous;		// Data left over from last pass

		/**********************************************************************\
		|* Private methods
		\**********************************************************************/
		void _allocate(void);

	public:
		/**********************************************************************\
		|* Constructor
		\**********************************************************************/
		explicit Processor(Config& cfg, QObject *parent = nullptr);

		/**********************************************************************\
		|* Initialise with the data-stream params
		\**********************************************************************/
		void init(SoapyIO *sio);

	public slots:
		void dataReceived(int handle, int samples, int max);

	};

#endif // PROCESSOR_H
