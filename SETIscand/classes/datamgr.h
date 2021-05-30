#ifndef DATAMGR_H
#define DATAMGR_H

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
		QVector<DataBlock*>			_active;		// List of in-use blocks
		QVector<DataBlock*>			_candidate;		// List of pending blocks

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
		int blockFor(int count, int sizePerElement);

		/**********************************************************************\
		|* Public Methods - return a pointer to the data in a given block
		\**********************************************************************/
		uint8_t * asUint8(int idx);
		int *	  asInt(int idx);
		float *   asFloat(int idx);
		double *  asDouble(int idx);

		/**********************************************************************\
		|* Public Methods - interface for retain counts from client side
		\**********************************************************************/
		int retainCount(int idx);
		void retain(int idx);
		void release(int idx);

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
