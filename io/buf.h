#ifndef __WLR__HUTTY__BUF__H
#define __WLR__HUTTY__BUF__H


#include <stdlib.h>
#include "../schedule/wlock.h"
#include "../utils/constant.h"


namespace wlr
{


#define W_TO_BYTE(buf, index, num) \
    buf[index] = (char)((num >> (index << 3)) & 255)
#define W_FROM_BYTE(buf, index, type) \
	(((type)(buf[index] & 255)) << (index << 3))


class ByteBuf
{
public:
    explicit ByteBuf(int capacity);
	ByteBuf(ByteBuf &buf, int len, int capacity = -1);
    ByteBuf(const char *buf, int len, int capacity = -1);
    ~ByteBuf();
    
    int readInt();	// read a int
    char readByte();
    bool readBool();
    short readShort();
	float readFloat();
	double readDouble();
    long long readLongLong();
	std::string readString(int len);
	int readBuf(ByteBuf* buf, int len = -1);
    int readBytes(char* buf, int len, bool free = false);
    int onlyReadBytes(char* buf, int len, bool free = false);

    int writeInt(int i);
    int writeByte(char c);
    int writeBool(bool b);
    int writeShort(short s);
	int writeFloat(float f);
	int writeDouble(double b);
    int writeLongLong(long long ll);
    int writeString(std::string str, int len = -1);
    int writeBuf(ByteBuf &buf, int len = -1);
	int writeBytes(const char *buf, int offset, int len, bool free = false);

	int write(char c);
	int write(bool b);
	int write(short s);
	int write(int i);
	int write(float f);
	int write(double d);
	int write(long long ll);
	int write(std::string str, int len = -1);
	int write(ByteBuf &buf, int len = -1);
	int write(const char *buf, int offset, int len, bool free = false);

    int readable();
    int writeable();
	int capacity();

    void clear();
    int resize(int capacity);

private:
    char *m_buf;
    int m_capacity;
    int m_read_index;
    int m_write_index;
	wlr::Mutex m_rmutex;
	wlr::Mutex m_wmutex;
};











}




#endif
