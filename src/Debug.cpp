#include <Debug.h>

const version mlibVersion = {VERSION_TAGS,VERSION_DATE};
version appVersion = {"NULL","NULL"};
int debugLevel = DEBUG_LEVEL_TRACE;

char date[30];
char * getDate(void){


	memset(date,0,30);
	time_t timer ;
	struct tm* timeinfo;
	time(&timer);
	timeinfo = localtime(&timer);
	strftime(date,29,"%Y%m%d-%T",localtime(&timer));
	return date;




}
void debugSetLevel(int level){
	debugLevel = level;
};


void hexDump(const void *_data, size_t size) {
    //我们要hexdump的内容的起始地址由*_data指定，大小为size
    const unsigned char *data = (const unsigned char *)_data;
    //获取起始地址位置
    unsigned int offset = 0;
    //偏移量初始化为0，也就是第一行最左边将显示0x0000
    while (offset < size) {
        printf(CYAN "0x%04x  " NONE, offset);
        //0xx  以四位十六进制的方式显示偏移量，如果不足四位的在左边补零，如0x0000--0x0010
        size_t n = size - offset;
        if (n > 16) {
            n = 16;
        }

        for (size_t i = 0; i < 16; ++i) {
            if (i == 8) {
                printf(" ");
            }

            if (offset + i < size) {
                printf(CYAN "%02x " NONE, data[offset + i]); // x以两位十六进制的方式输出内容
            } else {
                printf("   "); //如果数据已经不足16个，则以空格表示，以便对齐
            }
        }
        //for循环又来输出中间部分十六进制显示的内容，不多于16个一行，8个和8个之间空两格
        printf("| ");

        for (size_t i = 0; i < n; ++i) {
            if (isprint(data[offset + i])) {
                printf(CYAN "%c" NONE, data[offset + i]);
            } else {
                printf(".");
            }
        }
        //%c以字符的形式输出内容，如果是能够显示的字符，则显示，否则以 . 代替
        printf("\n");
        //每行只显示十六个字节
        offset += 16;
    }
}
