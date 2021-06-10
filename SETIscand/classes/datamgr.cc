#include "constants.h"
#include "datamgr.h"

/******************************************************************************\
|* Testing
\******************************************************************************/
#define MAX_TESTS (2)

/******************************************************************************\
|* Categorised logging support
\******************************************************************************/
Q_LOGGING_CATEGORY(log_data, "seti.data  ")

#define LOG qDebug(log_data) << QTime::currentTime().toString("hh:mm:ss.zzz")
#define ERR qCritical(log_data) << QTime::currentTime().toString("hh:mm:ss.zzz")

/******************************************************************************\
|* Create the data manager. Only called via the sharedInstance class method
\******************************************************************************/
DataMgr::DataMgr(void)
		:_handle(0)
	{}


/******************************************************************************\
|* Destroy the data blocks
\******************************************************************************/
DataMgr::~DataMgr(void)
	{
	for (DataBlock* block : _candidate)
		delete block;
	_candidate.clear();

	QMap<int64_t, DataBlock*>::const_iterator i = _active.constBegin();
		delete i.value();
	_active.clear();
	}

/******************************************************************************\
|* Create or find a block with a given size
\******************************************************************************/
int DataMgr::blockFor(size_t size)
	{
	int64_t result = (int64_t)-1;
	QMutexLocker guard(&_lock);

	/**************************************************************************\
	|* See if we can move one of the elements from the candidate list into the
	|* active list
	\**************************************************************************/
	bool foundBlock = false;
	int idx = 0;
	for (DataBlock* block : _candidate)
		{
		if (block->size() >= size)
			{
			result = _handle ++;
			_active[result] = block;
			_candidate.removeAt(idx);
			block->retain();
			foundBlock = true;
			break;
			}
		idx ++;
		}

	/**************************************************************************\
	|* If not, create a new block of sufficient size
	\**************************************************************************/
	if (!foundBlock)
		{
		result = _handle ++;
		DataBlock *block = new DataBlock(size);
		block->retain();
		_active[result] = block;
		result = _active.size()-1;
		}

	return result;
	}

/******************************************************************************\
|* Create or find a block with a given size-per-element and count
\******************************************************************************/
int DataMgr::blockFor(size_t count, size_t sizePerElement)
	{
	return blockFor(count * sizePerElement);
	}

/******************************************************************************\
|* Create or find a block with a given size using the FFTW3 allocation strategy
\******************************************************************************/
int DataMgr::fftBlockFor(size_t bins)
	{
	int64_t result = (int64_t)-1;
	QMutexLocker guard(&_lock);

	size_t size = sizeof(fftw_complex) * bins;

	/**************************************************************************\
	|* See if we can move one of the elements from the candidate list into the
	|* active list. Tighter constraints on the test due to the fft plan
	|* requirements
	\**************************************************************************/
	bool foundBlock = false;
	int idx = 0;
	for (DataBlock* block : _candidate)
		{
		if (block->size() == size && block->isFFT())
			{
			result = _handle ++;
			_active[result] = block;
			_candidate.removeAt(idx);
			block->retain();
			foundBlock = true;
			break;
			}
		idx ++;
		}

	/**************************************************************************\
	|* If not, create a new block of sufficient size using the FFTW allocator
	\**************************************************************************/
	if (!foundBlock)
		{
		result = _handle ++;
		DataBlock *block = new DataBlock(size, true);
		if (block == nullptr)
			result = -1;
		else
			{
			block->retain();
			_active[result] = block;
			}
		}

	return result;
	}


/******************************************************************************\
|* Return the allocation size of the block
\******************************************************************************/
size_t DataMgr::extent(int64_t idx)
	{
	QMutexLocker guard(&_lock);
	size_t extent = 0;
	if (_active.contains(idx))
		extent = _active[idx]->size();
	return extent;
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as uint8_t
\******************************************************************************/
uint8_t * DataMgr::asUint8(int64_t idx)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(idx))
		{
		ERR << "Requested uint8_t data for OOB index " << idx;
		return nullptr;
		}
	return _active[idx]->data();
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as int8_t
\******************************************************************************/
int8_t * DataMgr::asInt8(int64_t idx)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(idx))
		{
		ERR << "Requested int8_t data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<int8_t *>(_active[idx]->data());
	}


/******************************************************************************\
|* Return the pointer to the data in various formats: as uint16_t
\******************************************************************************/
uint16_t * DataMgr::asUint16(int64_t idx)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(idx))
		{
		ERR << "Requested uint16_t data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<uint16_t *>(_active[idx]->data());
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as int16_t
\******************************************************************************/
int16_t * DataMgr::asInt16(int64_t idx)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(idx))
		{
		ERR << "Requested uint8_t data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<int16_t *>(_active[idx]->data());
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as uint32_t
\******************************************************************************/
uint32_t * DataMgr::asUint32(int64_t idx)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(idx))
		{
		ERR << "Requested int data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<uint32_t *>(_active[idx]->data());
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as int32_t
\******************************************************************************/
int32_t * DataMgr::asInt32(int64_t idx)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(idx))
		{
		ERR << "Requested int data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<int32_t *>(_active[idx]->data());
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as float
\******************************************************************************/
float * DataMgr::asFloat(int64_t idx)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(idx))
		{
		ERR << "Requested float data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<float *>(_active[idx]->data());
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as double
\******************************************************************************/
double * DataMgr::asDouble(int64_t idx)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(idx))
		{
		ERR << "Requested double data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<double *>(_active[idx]->data());
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as complex float
\******************************************************************************/
std::complex<float> * DataMgr::asComplexFloat(int64_t idx)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(idx))
		{
		ERR << "Requested double data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<std::complex<float> *>(_active[idx]->data());
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as complex double
\******************************************************************************/
std::complex<double> * DataMgr::asComplexDouble(int64_t idx)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(idx))
		{
		ERR << "Requested double data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<std::complex<double> *>(_active[idx]->data());
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as fftw3 compatible
\******************************************************************************/
fftw_complex* DataMgr::asFFT(int64_t idx)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(idx))
		{
		ERR << "Requested FFT data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<fftw_complex*>(_active[idx]->data());
	}


/******************************************************************************\
|* Handle the retain-count for a given index
\******************************************************************************/
int DataMgr::retainCount(uint64_t handle)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(handle))
		{
		ERR << "Retain count requested for unknown handle " << handle;
		return -1;
		}
	return _active[handle]->refs();
	}

/******************************************************************************\
|* Handle release for a given index
\******************************************************************************/
void DataMgr::release(uint64_t handle)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(handle))
		{
		ERR << "Release requested for unknown handle " << handle;
		return;
		}
	_active[handle]->release();

	/**************************************************************************\
	|* Move to candidate if the refs == 0
	\**************************************************************************/
	if (_active[handle]->refs() == 0)
		{
		DataBlock *block = _active[handle];
		_candidate.append(block);
		_active.remove(handle);
		}
	}

/******************************************************************************\
|* Handle retain for a given index
\******************************************************************************/
void DataMgr::retain(uint64_t handle)
	{
	QMutexLocker guard(&_lock);

	if (!_active.contains(handle))
		{
		ERR << "Retain requested for unknown handle " << handle;
		}
	_active[handle]->retain();
	}

/******************************************************************************\
|* Test interface : return the number of tests we can run
\******************************************************************************/
int DataMgr::numTests(void)
	{
	return MAX_TESTS;
	}

/******************************************************************************\
|* Test interface : return the number of tests we can run
\******************************************************************************/
Testable::TestResult DataMgr::runTest(int idx)
	{
	switch (idx)
		{
		case 0:
			return _checkAllocations();
		case 1:
			return _checkRetainRelease();
		}

	ERR << "Test requested outside of range";
	return Testable::TEST_FAIL;
	}


/******************************************************************************\
|* Test interface : Check that we can allocate things
\******************************************************************************/
Testable::TestResult DataMgr::_checkAllocations(void)
	{
	_active.clear();
	_candidate.clear();

	int handle = blockFor(1024000);
	if (handle < 0)
		{
		ERR << "Cannot allocate 1024000 bytes";
		return Testable::TEST_FAIL;
		}

	if (_active.size() != 1)
		{
		ERR << "Block is not in active list";
		return Testable::TEST_FAIL;
		}

	if (_candidate.size() != 0)
		{
		ERR << "Candidate list is not empty";
		return Testable::TEST_FAIL;
		}

	// Clean up tidily
	release(handle);

	return Testable::TEST_PASS;
	}

/******************************************************************************\
|* Test interface : Check that we can allocate things
\******************************************************************************/
Testable::TestResult DataMgr::_checkRetainRelease(void)
	{
	_active.clear();
	_candidate.clear();

	int handle1 = blockFor(1024000);
	if (retainCount(handle1) != 1)
		{
		ERR << "Retain count invalid for " << handle1;
		return Testable::TEST_FAIL;
		}

	release(handle1);

	if (_active.size() != 0)
		{
		ERR << "Block was not moved to candidate list";
		return Testable::TEST_FAIL;
		}

	if (_candidate.size() != 1)
		{
		ERR << "Candidate list was not populated";
		return Testable::TEST_FAIL;
		}

	// Allocate another, larger block
	int handle2 = blockFor(2024000);
	if (retainCount(handle2) != 1)
		{
		ERR << "Retain count invalid for " << handle2;
		return Testable::TEST_FAIL;
		}

	if (_active.size() != 1)
		{
		ERR << "New block not created in active list";
		return Testable::TEST_FAIL;
		}

	if (_candidate.size() != 1)
		{
		ERR << "Candidate list corrupted";
		return Testable::TEST_FAIL;
		}

	// Now allocate another block of the initial size
	handle1 = blockFor(1024000);
	if (retainCount(handle1) != 1)
		{
		ERR << "Retain count invalid for " << handle1;
		return Testable::TEST_FAIL;
		}

	if (_active.size() != 2)
		{
		ERR << "New block not moved from candidate list";
		return Testable::TEST_FAIL;
		}

	if (_candidate.size() != 0)
		{
		ERR << "Candidate list not empty";
		return Testable::TEST_FAIL;
		}

	// Tidy up so we don't get warnings
	release(handle1);
	release(handle2);
	return Testable::TEST_PASS;
	}

/******************************************************************************\
|* Test interface : identify the class being tested
\******************************************************************************/
const char * DataMgr::testClassName(void)
	{
	return "DataMgr";
	}
