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
	GET(int, numIQ);						// Number of IQ points
	GET(int64_t, data);						// Buffer: Input to FFT
	GET(int64_t, results);					// Buffer: Output from FFT
	SET(fftw_plan, plan, Plan);				// FFT plan for fftw3
	GETSET(int64_t, window, Window);		// Buffer: FFT windowing data

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
