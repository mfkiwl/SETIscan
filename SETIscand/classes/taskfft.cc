#include "taskfft.h"

/******************************************************************************\
|* Constructor: single buffer
\******************************************************************************/
TaskFFT::TaskFFT(double *iq, int num)
		: QRunnable()
		, _numIQ(num/2)
		, _real(nullptr)
		, _imag(nullptr)
	{
	_real	= new double[_numIQ];
	_imag	= new double[_numIQ];

	double *I = _real;
	double *Q = _imag;
	for (int i=0; i<_numIQ; i++)
		{
		*I ++ = *iq ++;
		*Q ++ = *iq ++;
		}
	}

/******************************************************************************\
|* Constructor: two buffers
\******************************************************************************/
TaskFFT::TaskFFT(double *iq1, int num1, double *iq2, int num2)
		: QRunnable()
		, _numIQ((num1+num2)/2)
		, _real(nullptr)
		, _imag(nullptr)
	{
	_real	= new double[_numIQ];
	_imag	= new double[_numIQ];

	double *I = _real;
	double *Q = _imag;

	for (int i=0; i<num1/2; i++)
		{
		*I ++ = *iq1 ++;
		*Q ++ = *iq1 ++;
		}

	for (int i=0; i<num2/2; i++)
		{
		*I ++ = *iq2 ++;
		*Q ++ = *iq2 ++;
		}
	}

/******************************************************************************\
|* Destructor
\******************************************************************************/
TaskFFT::~TaskFFT(void)
	{
	if (_real) delete [] _real;
	if (_imag) delete [] _imag;
	}


/******************************************************************************\
|* Process the FFT
\******************************************************************************/
void TaskFFT::run(void)
	{
	fprintf(stderr, "Processing FFT\n");
	}
