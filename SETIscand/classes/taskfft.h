#ifndef TASKFFT_H
#define TASKFFT_H

#include <QRunnable>

#include "properties.h"

class TaskFFT : public QRunnable
	{
	GET(int, numIQ);
	GET(int64_t, data);

	public:
		TaskFFT(double *iq, int num);
		TaskFFT(double *iq1, int num1, double *iq2, int num2);
		~TaskFFT(void);

		void run() override;
	};

#endif // TASKFFT_H
