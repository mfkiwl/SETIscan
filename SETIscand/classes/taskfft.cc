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
		,_results(-1)
	{
	Q_ASSERT(num % 2 == 0);

	DataMgr &dmgr		= DataMgr::instance();

	_results			= dmgr.fftBlockFor(_numIQ);

	_data				= dmgr.fftBlockFor(_numIQ);
	fftw_complex *data	= dmgr.asFFT(_data);
	::memcpy(data, iq, _numIQ * sizeof(fftw_complex));
	}

/******************************************************************************\
|* Constructor: two buffers
\******************************************************************************/
TaskFFT::TaskFFT(double *iq1, int num1, double *iq2, int num2)
		: QRunnable()
		,_numIQ((num1+num2)/2)
		,_data(-1)
		,_results(-1)
	{
	DataMgr &dmgr		= DataMgr::instance();

	_results			= dmgr.fftBlockFor(_numIQ);

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
	DataMgr &dmgr		= DataMgr::instance();

	/**********************************************************************\
	|* Apply the windowing function to the data
	\**********************************************************************/
	double *window		= dmgr.asDouble(_window);
	fftw_complex *input	= dmgr.asFFT(_data);

	for (int i=0; i<_numIQ; i++)
		{
		input[i][0] *= window[i];
		input[i][1] *= window[i];
		}

	/**********************************************************************\
	|* Perform the FFT
	\**********************************************************************/
	fftw_execute_dft(_plan, dmgr.asFFT(_data), dmgr.asFFT(_results));

	/**********************************************************************\
	|* And tell the world we're done
	\**********************************************************************/
	emit fftDone(_results);
	}
