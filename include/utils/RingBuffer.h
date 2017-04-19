#ifndef RINGBUFFER_H
#define RINGBUFFER_H
#include <Debug.h>
#include <cstring>

#ifndef RB_MAX_LEN
#define RB_MAX_LEN 2048
#endif

//#define min(a, b) ( ( (a) < (b) ) ? (a) : (b) )
//#define min(X,Y) \
//	({  \
//	 typeof(X) __x=(X), __y=(Y);  \
//	 (__x<__y)?__x:__y;  \
//	 })
namespace mlib {
//#define min(a, b) (a)<(b)?(a):(b)


class RingBuffer
{
	public:
		RingBuffer(int size = RB_MAX_LEN);
		~RingBuffer();

		int availableRead();    //how much can read
		int availableWrite();   //how much can write
		int read(void *data, int count);  //read data frome ringbuffer
		int write(const void *data, int count);
		unsigned char getEntry(int offset);
		int findSymbol(char symbol);
		int findSymbol(const char *symbol);
		int size();

	private:
		int mBufferSize;       //buffer size
		char *mBufferBegin;
		/*环形缓冲区变量*/
		unsigned int mInputedData;
		unsigned int mOutputedData;

};
}
#endif // QRINGBUFFER_H
