#include <cstring>

#include "datamgr.h"
#include "taskfft.h"

/******************************************************************************\
|* Constructor: single buffer
\******************************************************************************/
TaskFFT::TaskFFT(double *iq, int num)
		: QRunnable()
		, _numIQ(num/2)
		, _data(-1)
	{
	DataMgr &dmgr		= DataMgr::instance();
	_data				= dmgr.fftBlockFor(_numIQ);
	fftw_complex *data	= dmgr.asFFT(_data);

	::memcpy(data, iq, _numIQ * sizeof(fftw_complex));
	}

/******************************************************************************\
|* Constructor: two buffers
\******************************************************************************/
TaskFFT::TaskFFT(double *iq1, int num1, double *iq2, int num2)
		: QRunnable()
		, _numIQ((num1+num2)/2)
		, _data(-1)
	{
	DataMgr &dmgr		= DataMgr::instance();
	_data				= dmgr.fftBlockFor(_numIQ);
	fftw_complex *data	= dmgr.asFFT(_data);

	memcpy(data, iq1, num1*sizeof(double));

	data += num1/2;
	memcpy(data, iq2, num2*sizeof(double));
	}

/******************************************************************************\
|* Destructor
\******************************************************************************/
TaskFFT::~TaskFFT(void)
	{
	if (_data >= 0)
		DataMgr::instance().release(_data);
	}


/******************************************************************************\
|* Process the FFT
\******************************************************************************/
void TaskFFT::run(void)
	{
	fprintf(stderr, "Processing FFT\n");
	}
