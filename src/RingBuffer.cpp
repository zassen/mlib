#include <utils/RingBuffer.h>
#include <algorithm>

/**
 *  * @brief RingBuffer::RingBuffer
 *   * @param buffersize Byte
 *    */
using namespace mlib;
using namespace std;
RingBuffer::RingBuffer(int size)
{
	bufferSize = size;
	rbCapacity = size;
	rbBuf = new unsigned char[bufferSize];
	rbBuff = rbBuf;
	rbHead = rbBuff;
	rbTail = rbBuff;
}

RingBuffer::~RingBuffer()
{
	rbBuff = NULL;
	rbHead = NULL;
	rbTail = NULL;
	rbCapacity = 0;
	delete []rbBuf; //释放缓冲区
}
/**
 *  * @brief RingBuffer::rbCanRead
 *   * @return 缓冲区可读字节数
 *    */
int RingBuffer::canRead()
{
	//ring buufer is null, return -1
	if((NULL == rbBuff)||(NULL == rbHead)||(NULL == rbTail))
	{
		return -1;
	}

	if (rbHead == rbTail)
	{
		return 0;
	}

	if (rbHead < rbTail)
	{
		return rbTail - rbHead;
	}
		return rbCapacity - (rbHead - rbTail);
	}

/**
* @brief RingBuffer::rbCanWrite  缓冲区剩余可写字节数
* @return  可写字节数
*/
int RingBuffer::canWrite()
{
	if((NULL == rbBuff)||(NULL == rbHead)||(NULL == rbTail))
	{
		return -1;
	}

	return rbCapacity - canRead();
}

/**
* @brief RingBuffer::read 从缓冲区读数据
* @param 目标数组地址
* @param 读的字节数
* @return
*/
int RingBuffer::read(void *data, int count)
{
	int copySz = 0;
	int canReadSize=0;

	if((NULL == rbBuff)||(NULL == rbHead)||(NULL == rbTail))
	{
		return -1;
	}
	if(NULL == data)
	{
		return -1;
	}

	if (rbHead < rbTail)
	{
		canReadSize = canRead();
		copySz = count<canReadSize?count:canReadSize;
		memcpy(data, rbHead, copySz);
		rbHead += copySz;
		return copySz;
	}
	else
	{
			if (count < rbCapacity-(rbHead - rbBuff))
		{
				copySz = count;
				memcpy(data, rbHead, copySz);
			rbHead += copySz;
			return copySz;
		}
		else
		{
			copySz = rbCapacity - (rbHead - rbBuff);
			memcpy(data, rbHead, copySz);
			rbHead = rbBuff;
			copySz += read((unsigned char *)data+copySz, count-copySz);
			return copySz;
		}
	}
}

/**
* @brief RingBuffer::write
* @param 数据地址
* @param 要写的字节数
* @return 写入的字节数
*/
int RingBuffer::write(const void *data, int count)
{
	int tailAvailSz = 0;

	if((NULL == rbBuff)||(NULL == rbHead)||(NULL == rbTail))
	{
		return -1;
	}

	if(NULL == data)
	{
		return -1;
	}

	if (count >= canWrite())
	{
		ASSERT("NO enough space to write");
		return -1;
	}

	if (rbHead <= rbTail)
	{
		tailAvailSz = rbCapacity - (rbTail - rbBuff);
		if (count <= tailAvailSz)
		{
			memcpy(rbTail, data, count);
			rbTail += count;
			if (rbTail == rbBuff+rbCapacity)
			{
				rbTail = rbBuff;
			}
				return count;
		}
		else
		{
			memcpy(rbTail, data, tailAvailSz);
			rbTail = rbBuff;

			return tailAvailSz + write((char*)data+tailAvailSz, count-tailAvailSz);
		}
	}
	else
	{
		memcpy(rbTail, data, count);
		rbTail += count;

		return count;
	}
}

/**
* @brief RingBuffer::size
* @return 缓冲区大小
*/
int RingBuffer::size()
{
	return bufferSize;
}

unsigned char* RingBuffer::findSymbol(unsigned char symbol){
	int copySz = 0;
	unsigned char *result = 0;
	int canReadSize=0;
	int tailAvailSz ;
	if((NULL == rbBuff)||(NULL == rbHead)||(NULL == rbTail))
	{
		return NULL;
	}
	if (rbHead <= rbTail){
		tailAvailSz = rbCapacity - (rbTail - rbBuff);
		result = find(rbTail,rbTail+tailAvailSz,symbol);
	}

}
