#include "buf.h"


wlr::ByteBuf::ByteBuf(int capacity)
    : m_capacity(capacity)
    , m_read_index(0)
    , m_write_index(0)
{
    this->m_buf = (char*)malloc(1 * capacity); 
}

wlr::ByteBuf::ByteBuf(const char *buf, int len, int capacity)
    : wlr::ByteBuf::ByteBuf(capacity == -1 ? len : capacity)
{
    this->writeBytes(buf, 0, len);
}

wlr::ByteBuf::ByteBuf(ByteBuf &buf, int len, int capacity)
    : wlr::ByteBuf::ByteBuf(capacity == -1 ? len : capacity)
{
    this->writeBuf(buf, len);
}

wlr::ByteBuf::~ByteBuf()
{
	// printf("-----------wlr::ByteBuf::~ByteBuf()---------\n");
    free(this->m_buf);
}

#define W_DATA_MERGE(buf, size, type, num) \
	W_FOR(i,<,size) num |= W_FROM_BYTE(buf, i, type);

int wlr::ByteBuf::readInt()
{
    int r = -1;
	char buf[4];
	int len = this->readBytes(buf, 4, true);
	if (len != -1)
	{
		r = 0;
		W_DATA_MERGE(buf, 4, int, r)
	}
    return r;
}

char wlr::ByteBuf::readByte()
{
	char buf[1] = {'\000'};
	this->readBytes(buf, 1, true);
	return buf[0];
}

bool wlr::ByteBuf::readBool()
{
	char buf[1] = {(char)0};
	this->readBytes(buf, 1, true);
	return buf[0] != 0;
}

short wlr::ByteBuf::readShort()
{
	short s = -1;
	char buf[2];
	int len = this->readBytes(buf, 2, true);
	if (len != 1)
	{
		s = 0;
		W_DATA_MERGE(buf, 2, short, s)
	}
	return s;
}

long long wlr::ByteBuf::readLongLong()
{
	long long ll = -1;
	char buf[8];
	int len = this->readBytes(buf, 8, true);
	if (len != -1)
	{
		ll = 0;
		W_DATA_MERGE(buf, 8, long long, ll)
	}
	return ll;
}

float wlr::ByteBuf::readFloat()
{
	float f = 0.0f;
	float *fp = &f;
	char buf[4];
	int len = this->readBytes(buf, 4, true);
	if (len != -1) fp = (float*)buf;
	return *fp;
}

double wlr::ByteBuf::readDouble()
{
	double d = 0.0;
	double *dp = &d;
	char buf[8];
	int len = this->readBytes(buf, 8, true);
	if (len != -1) dp = (double*)buf;
	return *dp;
}

int wlr::ByteBuf::readBytes(char *buf, int len, bool free)
{
	this->m_rmutex.lock();
	int read = 0;

	do 
	{
		int readable;
		if (free && (readable = this->readable()) < len) break;
		while(++read <= len && this->readable() > 0)
		{
			*buf = this->m_buf[this->m_read_index++];
			 buf++;
		}	
	} 
	while(0);

	this->m_rmutex.unlock();
	return read - 1;
}

std::string wlr::ByteBuf::readString(int len)
{
	std::string str;
	if (len < 1) return str;
	char buf[len];
	W_FOR(i,<,len) buf[i] = '\000';
	len = this->readBytes(buf, len, true);
	if (len != -1) str.append(buf, len);
	return str;
}

int wlr::ByteBuf::readBuf(ByteBuf *buf, int len)
{
	if (len < 1) {
		int r, w;
		len = (r = this->readable()) > (w = buf->writeable()) ? w : r;
	}

	char buffer[len];
	len = this->readBytes(buffer, len, true);
	len = buf->writeBytes(buffer, 0, len, true);
	return len;
}

#define W_WEITE_BYTE(size, num, var) \
	char buf[size]; \
	W_FOR(index, <, size) W_TO_BYTE(buf, index, num); \
	int var = this->writeBytes(buf, 0, size, true);

int wlr::ByteBuf::writeInt(int i)
{
	W_WEITE_BYTE(4, i, len)
	return len;
}

int wlr::ByteBuf::writeByte(char c)
{
	char buf[1] = {c};
	return this->writeBytes(buf, 0, 1, true);
}

int wlr::ByteBuf::writeBool(bool b)
{
	char buf[1] = { (char)(b ? 1 : 0) };
	return this->writeBytes(buf, 0, 1, true);
}

int wlr::ByteBuf::writeShort(short s)
{
	W_WEITE_BYTE(2, s, len)
	return len;
}

int wlr::ByteBuf::writeLongLong(long long ll)
{
	W_WEITE_BYTE(8, ll, len)
	return len;
}

int wlr::ByteBuf::writeFloat(float f)
{
	float *fp = &f;
	char *buf = (char*)fp;
	return this->writeBytes(buf, 0, 4, true);
}

int wlr::ByteBuf::writeDouble(double d)
{
	double *dp = &d;
	char *buf = (char*)dp;
	return this->writeBytes(buf, 0, 8, true);
}

int wlr::ByteBuf::writeBytes(const char *buf, int offset, int len, bool free)
{
	this->m_wmutex.lock();
	int write = 0;

	do
	{
		int writeable;
		if (offset < 0 || len < 1) break;
		if (free && (writeable = this->writeable()) < len) break;

		while (++write <= len && this->writeable() > 0)
			this->m_buf[this->m_write_index++] = buf[offset++];
	}
	while(0);

	this->m_wmutex.unlock();
	return write - 1;
}

int wlr::ByteBuf::writeString(std::string str, int len)
{
	if (len < 1) len = str.size(); 
	const char *buf = str.c_str();
	len = this->writeBytes(buf, 0, len, true);
	return len;
}

int wlr::ByteBuf::writeBuf(ByteBuf &buf, int len)
{
	if (len < 1) {
		int r, w;
		len = (r = buf.readable()) > (w = this->writeable()) ? w : r;
	}
	
	char buffer[len];
	len = buf.readBytes(buffer, len, true);
	len = this->writeBytes(buffer, 0, len, true);
	return len;
}

int wlr::ByteBuf::write(char c)
{ return this->writeByte(c); }

int wlr::ByteBuf::write(bool b)
{ return this->writeBool(b); }

int wlr::ByteBuf::write(short s)
{ return this->writeShort(s); }

int wlr::ByteBuf::write(int i)
{ return this->writeInt(i); }

int wlr::ByteBuf::write(float f)
{ return this->writeFloat(f); }

int wlr::ByteBuf::write(double d)
{ return this->writeDouble(d); }

int wlr::ByteBuf::write(long long ll)
{ return this->writeLongLong(ll); }

int wlr::ByteBuf::write(std::string str, int len)
{ return this->writeString(str, len); }

int wlr::ByteBuf::write(ByteBuf &buf, int len)
{ return this->writeBuf(buf, len); }

int wlr::ByteBuf::write(const char *buf, int offset, int len, bool free)
{ return this->writeBytes(buf, offset, len, free); }

int wlr::ByteBuf::readable()
{
    return this->m_write_index - this->m_read_index;
}

int wlr::ByteBuf::writeable()
{
    return this->m_capacity - this->m_write_index;
}

int wlr::ByteBuf::capacity()
{
    return this->m_capacity;
}

void wlr::ByteBuf::clear()
{
    this->m_read_index = 0;
    this->m_write_index = 0;
}

int wlr::ByteBuf::resize(int capacity)
{
	if (capacity < 1) return -1;
	this->m_rmutex.lock();
	this->m_wmutex.lock();

	int change = this->m_capacity - capacity;
	if (capacity == this->m_capacity) return 0;

	if (capacity > this->m_capacity) {
		char* newBuf = (char*)malloc(1 * capacity);
		W_FOR(i, < , this->m_capacity) newBuf[i] = this->m_buf[i];
		free(this->m_buf);
		this->m_capacity = capacity;
		this->m_buf = newBuf;
	} else {
		this->m_capacity = capacity;
		if (this->m_read_index > this->m_capacity) this->m_read_index = this->m_capacity;
		if (this->m_write_index > this->m_capacity) this->m_write_index = this->m_capacity;
	}

	this->m_wmutex.unlock();
	this->m_rmutex.unlock();
    return change;
}

#undef W_WRITE_BYTE
#undef W_DATA_MERGE
