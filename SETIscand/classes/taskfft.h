#ifndef TASKFFT_H
#define TASKFFT_H

#include <fftw3.h>

#include <QObject>
#include <QRunnable>

#include "properties.h"

class TaskFFT : public QObject, public QRunnable
	{
	Q_OBJECT

	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GET(int, numIQ);
	GET(int64_t, data);
	GET(int64_t, results);
	SET(fftw_plan, plan, Plan);

	public:
		/**********************************************************************\
		|* Constructors and destructor
		\**********************************************************************/
		TaskFFT(double *iq, int num);
		TaskFFT(double *iq1, int num1, double *iq2, int num2);
		~TaskFFT(void);

		/**********************************************************************\
		|* Method called to run the task
		\**********************************************************************/
		void run() override;

	signals:
		/**********************************************************************\
		|* FFT done, please aggregate this data
		\**********************************************************************/
		void fftDone(int bufferId);
	};

#endif // TASKFFT_H
