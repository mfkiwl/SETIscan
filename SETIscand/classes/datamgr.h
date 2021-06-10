#ifndef DATAMGR_H
#define DATAMGR_H

#include <complex>
#include <fftw3.h>

#include <QMap>
#include <QMutexLocker>
#include <QVector>

#include "properties.h"
#include "datablock.h"
#include "singleton.h"
#include "testable.h"

class DataMgr : public Singleton<DataMgr>, public Testable
	{
	NON_COPYABLE_NOR_MOVEABLE(DataMgr);

	private:
		/**********************************************************************\
		|* Private variables
		\**********************************************************************/
		QMutex						_lock;			// Thread safety
		int64_t						_handle;		// Constantly increasing
		QMap<int64_t, DataBlock*>	_active;		// Map of in-use blocks
		QVector<DataBlock*>			_candidate;		// List of candidate blocks

	public:
		/**********************************************************************\
		|* Constructor
		\**********************************************************************/
		explicit DataMgr(void);
		~DataMgr(void);

		/**********************************************************************\
		|* Public Methods - return a handle for a block of a given size
		\**********************************************************************/
		int blockFor(size_t size);
		int blockFor(size_t count, size_t sizePerElement);

		/**********************************************************************\
		|* This will allocate the block using fftw3 memory allocation
		\**********************************************************************/
		int fftBlockFor(size_t bins);

		/**********************************************************************\
		|* Public Methods - return a pointer to the data in a given block
		\**********************************************************************/
		uint8_t *				asUint8(int64_t idx);
		int8_t *				asInt8(int64_t idx);
		uint16_t *				asUint16(int64_t idx);
		int16_t *				asInt16(int64_t idx);
		uint32_t *				asUint32(int64_t idx);
		int32_t *				asInt32(int64_t idx);
		float *					asFloat(int64_t idx);
		double *				asDouble(int64_t idx);
		std::complex<float> *	asComplexFloat(int64_t idx);
		std::complex<double> *	asComplexDouble(int64_t idx);
		fftw_complex *			asFFT(int64_t idx);

		/**********************************************************************\
		|* Public Method - return the allocation size of the block or 0
		\**********************************************************************/
		size_t extent(int64_t idx);

		/**********************************************************************\
		|* Public Methods - interface for retain counts from client side
		\**********************************************************************/
		int retainCount(uint64_t handle);
		void retain(uint64_t handle);
		void release(uint64_t handle);

		/**********************************************************************\
		|* Public Tests interface
		\**********************************************************************/
		int numTests(void);
		Testable::TestResult runTest(int idx);
		const char * testClassName(void);

	private:
		/**********************************************************************\
		|* Private Tests
		\**********************************************************************/
		Testable::TestResult _checkAllocations(void);
		Testable::TestResult _checkRetainRelease(void);

	};

#endif // DATAMGR_H
