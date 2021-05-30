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
	{}


/******************************************************************************\
|* Destroy the data blocks
\******************************************************************************/
DataMgr::~DataMgr(void)
	{
	for (DataBlock* block : _candidate)
		delete block;
	_candidate.clear();

	for (DataBlock* block : _active)
		delete block;
	_active.clear();
	}

/******************************************************************************\
|* Create or find a block with a given size
\******************************************************************************/
int DataMgr::blockFor(size_t size)
	{
	int result = -1;
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
			_active.append(block);
			_candidate.removeAt(idx);
			block->retain();
			foundBlock = true;
			result = _active.size()-1;
			break;
			}
		idx ++;
		}

	/**************************************************************************\
	|* If not, create a new block of sufficient size
	\**************************************************************************/
	if (!foundBlock)
		{
		DataBlock *block = new DataBlock(size);
		block->retain();
		_active.append(block);
		result = _active.size()-1;
		}

	return result;
	}

/******************************************************************************\
|* Create or find a block with a given size-per-element and count
\******************************************************************************/
int DataMgr::blockFor(int count, int sizePerElement)
	{
	return blockFor(count * sizePerElement);
	}


/******************************************************************************\
|* Return the pointer to the data in various formats: as uint8_t
\******************************************************************************/
uint8_t * DataMgr::asUint8(int idx)
	{
	QMutexLocker guard(&_lock);

	if ((idx < 0) || (idx >= _active.size()))
		{
		ERR << "Requested uint8_t data for OOB index " << idx;
		return nullptr;
		}
	return _active[idx]->data();
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as double
\******************************************************************************/
int * DataMgr::asInt(int idx)
	{
	QMutexLocker guard(&_lock);

	if ((idx < 0) || (idx >= _active.size()))
		{
		ERR << "Requested int data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<int *>(_active[idx]->data());
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as float
\******************************************************************************/
float * DataMgr::asFloat(int idx)
	{
	QMutexLocker guard(&_lock);

	if ((idx < 0) || (idx >= _active.size()))
		{
		ERR << "Requested float data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<float *>(_active[idx]->data());
	}

/******************************************************************************\
|* Return the pointer to the data in various formats: as double
\******************************************************************************/
double * DataMgr::asDouble(int idx)
	{
	QMutexLocker guard(&_lock);

	if ((idx < 0) || (idx >= _active.size()))
		{
		ERR << "Requested double data for OOB index " << idx;
		return nullptr;
		}
	return reinterpret_cast<double *>(_active[idx]->data());
	}


/******************************************************************************\
|* Handle the retain-count for a given index
\******************************************************************************/
int DataMgr::retainCount(int idx)
	{
	QMutexLocker guard(&_lock);

	if ((idx < 0) || (idx >= _active.size()))
		{
		ERR << "Retain count requested for OOB index " << idx;
		return -1;
		}
	return _active[idx]->refs();
	}

/******************************************************************************\
|* Handle release for a given index
\******************************************************************************/
void DataMgr::release(int idx)
	{
	QMutexLocker guard(&_lock);

	if ((idx < 0) || (idx >= _active.size()))
		{
		ERR << "Release requested for OOB index " << idx;
		return;
		}
	_active[idx]->release();

	/**************************************************************************\
	|* Move to candidate if the refs == 0
	\**************************************************************************/
	if (_active[idx]->refs() == 0)
		{
		DataBlock *block = _active[idx];
		_candidate.append(block);
		_active.removeAt(idx);
		}
	}

/******************************************************************************\
|* Handle retain for a given index
\******************************************************************************/
void DataMgr::retain(int idx)
	{
	QMutexLocker guard(&_lock);

	if ((idx < 0) || (idx >= _active.size()))
		{
		ERR << "Retain requested for OOB index " << idx;
		}
	_active[idx]->retain();
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
