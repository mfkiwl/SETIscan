#ifndef FFTAGGREGATOR_H
#define FFTAGGREGATOR_H

#include <QMutexLocker>
#include <QObject>

#include "properties.h"

class FFTAggregator : public QObject
	{
	Q_OBJECT

	public:
		/**********************************************************************\
		|* Enums and Typedefs
		\**********************************************************************/
		typedef enum
			{
			TYPE_NONE	= 0,
			TYPE_UPDATE,
			TYPE_SAMPLE
			} DataType;

	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GET(int, fftSize);					// Bins in the FFT
	GET(bool, haveData);				// Whether we've received any data yet
	GET(double, updateSecs);			// Seconds between updates
	GET(double, sampleSecs);			// Seconds between samples
	GET(qint64, nextUpdate);			// Next time to deliver an update
	GET(qint64, nextSample);			// Next time to deliver a sample
	GET(int, updatePasses);				// Count of update aggregations
	GET(int, samplePasses);				// Count of sample aggregations


	private:
		/**********************************************************************\
		|* Private variables
		\**********************************************************************/
		QMutex			_lock;			// Thread safety
		double *		_updateData;	// Results of the update aggregation
		double *		_sampleData;	// Results of the sample aggregation

		/**********************************************************************\
		|* Private methods
		\**********************************************************************/
		qint64 _deltaT(double delta);

	signals:
		/**********************************************************************\
		|* Tell the world we have new data it might want to use
		\**********************************************************************/
		void aggregatedDataReady(DataType type, int buffer);

	public:
		/**********************************************************************\
		|* Constructor
		\**********************************************************************/
		explicit FFTAggregator(QObject *parent = nullptr);
		~FFTAggregator(void);

	public slots:
		/**********************************************************************\
		|* Receive an FFT buffer from a worker
		\**********************************************************************/
		void fftReady(int bufferId);

	};

#endif // FFTAGGREGATOR_H
