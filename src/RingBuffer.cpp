#include <utils/RingBuffer.h>

/**
 *  * @brief RingBuffer::RingBuffer
 *   * @param buffersize Byte
 *    */
using namespace mlib;
using namespace std;
RingBuffer::RingBuffer(int size)
{
	mBufferSize = size;
	mBufferBegin = new unsigned char[mBufferSize];
	mInputedData = 0;
	mOutputedData = 0;
}

RingBuffer::~RingBuffer()
{
	mBufferBegin = NULL;
	mBufferSize = 0;
	delete []mBufferBegin; //释放缓冲区
}
/**
 *  * @brief RingBuffer::rbCanRead
 *   * @return 缓冲区可读字节数
 *    */
int RingBuffer::availableRead()
{
	//ring buufer is null, return -1
	if(mBufferBegin == NULL)ASSERT("buffer uninited");
	
	return (mInputedData - mOutputedData);

}

/**
* @brief RingBuffer::rbCanWrite  缓冲区剩余可写字节数
* @return  可写字节数
*/
int RingBuffer::availableWrite()
{
	if(mBufferBegin == NULL)ASSERT("buffer uninited");

	return (mBufferSize - availableRead());
}

/**
* @brief RingBuffer::read 从缓冲区读数据
* @param 目标数组地址
* @param 读的字节数
* @return
*/
int RingBuffer::read(void *data, int count)
{
	int tailLen=0;
	int realOutput=0;
	int tmpCanRead=0;
	if(mBufferBegin == NULL)ASSERT("buffer uninited");
	if(NULL == data)ASSERT("Input data is unavailable");
	tmpCanRead = availableRead();
	count =min(count,tmpCanRead);
	realOutput = mOutputedData & ( mBufferSize -1 );   //get real data out index
	tailLen = mBufferSize - realOutput;
	tailLen = min( count, tailLen );
	memcpy( (unsigned char*)data, mBufferBegin+realOutput, tailLen);
	memcpy( (unsigned char*)data+tailLen, mBufferBegin, count-tailLen);
	mOutputedData += count;
	return count;
	

}

/**
* @brief RingBuffer::write
* @param 数据地址
* @param 要写的字节数
* @return 写入的字节数
*/
int RingBuffer::write(const void *data, int count)
{

	int tailLen=0;
	int realInput=0;
	int tmpCanWrite=0;
	if(mBufferBegin == NULL)ASSERT("buffer uninited");
	if(NULL == data)ASSERT("Input data is unavailable");
	tmpCanWrite = availableWrite();
	count = min(count, tmpCanWrite);
	realInput = mInputedData & ( mBufferSize - 1 );
	tailLen = mBufferSize - realInput;
	tailLen = min(count, tailLen);
	memcpy(mBufferBegin + realInput, (unsigned char*)data, tailLen);
	memcpy(mBufferBegin, (unsigned char*)data+tailLen, count - tailLen);
	mInputedData += count;
	return count;
}

/**
* @brief RingBuffer::size
* @return 缓冲区大小
*/
int RingBuffer::size()
{
	return mBufferSize;
}

int RingBuffer::findSymbol(unsigned char symbol){
	int result = 0;
	int tmpOutputData = 0;
	unsigned char *targetAddress = 0;
	int canFindSize=0;
	int realFindIndex; 
	if(mBufferBegin == NULL)ASSERT("buffer uninited");
	tmpOutputData = mOutputedData;
	realFindIndex = mOutputedData & ( mBufferSize -1 );   //get real data out index
	targetAddress = mBufferBegin+realFindIndex;
	canFindSize = availableRead();
	TRACE("tmpOutputData:%d",tmpOutputData);
	TRACE("realFindIndex:%d",realFindIndex);
	if(*targetAddress == symbol){
			TRACE("symbol:%c, target:%c", symbol, *targetAddress);
			TRACE("result:%d, canFindSize:%d", result, canFindSize);
			return 1;
	}
	do{

		targetAddress = mBufferBegin+realFindIndex;
		if(*targetAddress == symbol){
			TRACE("symbol:%c, target:%c", symbol, *targetAddress);
			TRACE("result:%d, canFindSize:%d",result, canFindSize);
			return result;
		}
		result++;	
		realFindIndex = (tmpOutputData + result)  & ( mBufferSize -1 );   //get real data out index
		canFindSize--;
	}while(canFindSize != 0);
	TRACE("symbol:%c, target:%c", symbol, *targetAddress);
	TRACE("result:%d, canFindSize:%d", result, canFindSize);
	return 0;
	

}
