#include "FileDescriptor.h"
#include "Kernel.h"
#include <stdlib.h>
#include <string.h>

FileDescriptor::FileDescriptor(FileSystem * newFileSystem, IndexNode &newIndexNode, int newFlags)
{
	deviceNumber = -1 ;
	indexNodeNumber = -1 ;
	offset = 0 ;

	flags = newFlags ;
	fileSystem = newFileSystem;

	//copy index node info
	newIndexNode.copy(indexNode);
	bytes = new char[fileSystem->getBlockSize()];
	memset(bytes, '\0', fileSystem->getBlockSize());
}

FileDescriptor::~FileDescriptor()
{
	if(bytes != NULL)
	{
		delete[] bytes;
	}
}

void FileDescriptor::setDeviceNumber(short newDeviceNumber)
{
	deviceNumber = newDeviceNumber;
}

short FileDescriptor::getDeviceNumber()
{
	return deviceNumber;
}

IndexNode * FileDescriptor::getIndexNode()
{
	return &indexNode;
}

void FileDescriptor::setIndexNodeNumber(short newIndexNodeNumber)
{
	indexNodeNumber = newIndexNodeNumber;
}

short FileDescriptor::getIndexNodeNumber()
{
	return indexNodeNumber;
}

int FileDescriptor::getFlags()
{
	return flags;
}

char * FileDescriptor::getBytes()
{
	return bytes;
}

short FileDescriptor::getMode()
{
	return indexNode.getMode();
}

int FileDescriptor::getSize()
{
	return indexNode.getSize();
}

void FileDescriptor::setSize(int newSize)
{
	indexNode.setSize(newSize);

	// write the inode
	fileSystem->writeIndexNode(&indexNode, indexNodeNumber);
}

short FileDescriptor::getBlockSize()
{
	return fileSystem->getBlockSize();
}

int FileDescriptor::getOffset()
{
	return offset;
}

void FileDescriptor::setOffset(int newOffset)
{
	offset = newOffset;
}

int FileDescriptor::readBlock(short relativeBlockNumber)
{
	if(relativeBlockNumber >= IndexNode::MAX_FILE_BLOCKS)
	{
		Kernel::setErrno(Kernel::EFBIG);
		return -1 ;
	}
	// ask the IndexNode for the actual block number
	// given the relative block number
	int blockOffset = indexNode.getBlockAddress(relativeBlockNumber);
	int blockSize = fileSystem->getBlockSize();
	
	if(blockOffset == FileSystem::NOT_A_BLOCK)
	{
		// clear the bytes if it's a block that was never written
		for(int i=0;i<blockSize;i++)
		{
			bytes[i] = (char)0 ;
		}
	}
	else
	{
		memset(bytes, '\0', blockSize);
		// read the actual block into bytes
		
                if(relativeBlockNumber >= IndexNode::MAX_DIRECT_BLOCKS)
		{
			//Read contents of indirect block
			char * temp_bytes = new char[fileSystem->getBlockSize()];
			memset(temp_bytes, '\0', fileSystem->getBlockSize());
			fileSystem->read(temp_bytes, fileSystem->getDataBlockOffset() + blockOffset);
			int indirectOffset = (relativeBlockNumber - IndexNode::MAX_DIRECT_BLOCKS) * 3;
			int b2 = temp_bytes[indirectOffset] & 0xff;
			int b1 = temp_bytes[indirectOffset+1] & 0xff;
			int b0 = temp_bytes[indirectOffset+2] & 0xff;
			int indirectBlockNumber = b2 << 16 | b1 << 8 | b0 ;

			fileSystem->read(bytes, fileSystem->getDataBlockOffset() + indirectBlockNumber);
		}
		else
		{
			fileSystem->read(bytes, fileSystem->getDataBlockOffset() + blockOffset);
		}
	}
	return 0 ;
}

void FileDescriptor::initIndirectBlock(int indirectBlockOffset)
{
	int blockSize = fileSystem->getBlockSize();
	int nblocks = blockSize / 3;
	char * buffer;
	buffer = new char[fileSystem->getBlockSize()];
	memset(buffer, '\0', fileSystem->getBlockSize());
	for(int i = 0; i < nblocks; i++ )
	{
            buffer[3*i]   = (unsigned char)(FileSystem::NOT_A_BLOCK >> 16);
            buffer[3*i+1] = (unsigned char)(FileSystem::NOT_A_BLOCK >> 8);
            buffer[3*i+2] = (unsigned char)(FileSystem::NOT_A_BLOCK);
	}

	fileSystem->write(buffer, fileSystem->getDataBlockOffset() + indirectBlockOffset);
}

int FileDescriptor::writeBlock(short relativeBlockNumber)
{
	if(relativeBlockNumber >= IndexNode::MAX_FILE_BLOCKS)
	{
		Kernel::setErrno( Kernel::EFBIG ) ;
		return -1 ;
	}

	// ask the IndexNode for the actual block number
	// given the relative block number

	int blockOffset = indexNode.getBlockAddress(relativeBlockNumber);

	if(blockOffset == FileSystem::NOT_A_BLOCK)
	{
		// allocate a block; quit if we can't
		blockOffset = fileSystem->allocateBlock() ;

		if( blockOffset < 0 )
		{
			return -1 ;
		}

		if(relativeBlockNumber < IndexNode::MAX_DIRECT_BLOCKS)
		{
			// update the inode
			indexNode.setBlockAddress(relativeBlockNumber, blockOffset);
		}
		else
		{
			indexNode.setIndirectBlock(blockOffset);
			initIndirectBlock(blockOffset);
		}
		// write the inode
		fileSystem->writeIndexNode(&indexNode, indexNodeNumber);
	}

	if(relativeBlockNumber >= IndexNode::MAX_DIRECT_BLOCKS)
	{
		char * temp_bytes = new char[fileSystem->getBlockSize()];
		memset(temp_bytes, '\0', fileSystem->getBlockSize());
		fileSystem->read(temp_bytes, fileSystem->getDataBlockOffset() + blockOffset);
		int indirectOffset = (relativeBlockNumber - IndexNode::MAX_DIRECT_BLOCKS) * 3;

		int b2 = temp_bytes[indirectOffset] & 0xff;
		int b1 = temp_bytes[indirectOffset+1] & 0xff;
		int b0 = temp_bytes[indirectOffset+2] & 0xff;
		int indirectBlockNumber = b2 << 16 | b1 << 8 | b0 ;

		if(indirectBlockNumber == FileSystem::NOT_A_BLOCK)
		{
			int newBlock = fileSystem->allocateBlock();
			if( newBlock < 0)
			{
				return -1;
			}

			temp_bytes[indirectOffset] = (unsigned char)(newBlock >> 16);
                        temp_bytes[indirectOffset+1] = (unsigned char)(newBlock >> 8);
                        temp_bytes[indirectOffset+2] = (unsigned char)(newBlock);

			fileSystem->write(temp_bytes, fileSystem->getDataBlockOffset() + blockOffset);
			blockOffset = newBlock;
		}
		else
		{
			blockOffset = indirectBlockNumber;
		}
	}

		// write the actual block from bytes
	fileSystem->write(bytes, fileSystem->getDataBlockOffset() + blockOffset);
	return 0 ;
}
