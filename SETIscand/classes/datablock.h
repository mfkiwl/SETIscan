#ifndef DATABLOCK_H
#define DATABLOCK_H

#include <QObject>

#include "properties.h"

class DataBlock
	{
	/**************************************************************************\
	|* Properties
	\**************************************************************************/
	GET(size_t, size);				// Size of the block in bytes
	GET(uint8_t *, data);			// Actual data block
	GET(int, refs);					// Number of clients for this block
	GET(bool, isValid);				// If the block is valid post construction

	public:
		/**********************************************************************\
		|* Construction
		\**********************************************************************/
		explicit DataBlock(int elements, int sizePerElement);
		explicit DataBlock(size_t size);
		~DataBlock();

		/**********************************************************************\
		|* Retain the block, marking it as in-use by some client
		\**********************************************************************/
		void retain(void);

		/**********************************************************************\
		|* Release the block, marking it as no-longer-in-use by some client
		\**********************************************************************/
		void release(void);

	};

#endif // DATABLOCK_H
