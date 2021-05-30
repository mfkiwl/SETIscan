#include "constants.h"
#include "datablock.h"

/******************************************************************************\
|* Categorised logging support
\******************************************************************************/
#define LOG qDebug(log_data) << QTime::currentTime().toString("hh:mm:ss.zzz")
#define ERR qCritical(log_data) << QTime::currentTime().toString("hh:mm:ss.zzz")


/******************************************************************************\
|* Construct a fixed-size block
\******************************************************************************/
DataBlock::DataBlock(size_t size)
		  :_size(size)
		  ,_data(nullptr)
		  ,_refs(0)
		  ,_isValid(false)
	{
	_data = new uint8_t [_size];
	if (_data != nullptr)
		_isValid = true;
	}

/******************************************************************************\
|* Construct a fixed-size block
\******************************************************************************/
DataBlock::DataBlock(int elements, int sizePerElement)
		  :_size(elements * sizePerElement)
		  ,_data(nullptr)
		  ,_refs(0)
		  ,_isValid(false)
	{
	_data = new uint8_t [_size];
	if (_data != nullptr)
		_isValid = true;
	}


/******************************************************************************\
|* Destroy a block
\******************************************************************************/
DataBlock::~DataBlock(void)
	{
	if (_refs != 0)
		ERR << "Warning - deleting non-zero-references block!";
	if (_data != nullptr)
		delete [] _data;
	}

/******************************************************************************\
|* Retain a block to say it's in use
\******************************************************************************/
void DataBlock::retain(void)
	{
	_refs ++;
	}

/******************************************************************************\
|* Retain a block to say it's in use
\******************************************************************************/
void DataBlock::release(void)
	{
	if (_refs <= 0)
		{
		ERR << "Asked to de-ref data-block with ref count " << _refs;
		_refs = 0;
		}
	else
		_refs --;
	}
