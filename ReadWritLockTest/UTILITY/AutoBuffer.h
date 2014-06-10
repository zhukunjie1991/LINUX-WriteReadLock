// malloc与new一样在堆中分配，因此，可以申请到10M的内存空间

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <string>

class AutoBuffer
{
public:
    static const int InitialSize = 4096;
    AutoBuffer()
            : FixMultiple(2),
              m_readerIndex(0),
              m_writerIndex(0),
              m_capacity(InitialSize)
    {
		// tbd: 确保申请到空间
		m_buffer = (char *)malloc(InitialSize);
		memset(m_buffer, 0, InitialSize);
	}

    ~AutoBuffer()
    {
		free(m_buffer);
	}

    int ReadableBytes() const
    {
        return m_writerIndex - m_readerIndex;
    }

    int WritableBytes() const
    {
        return m_capacity - m_writerIndex;
    }

    char* Begin()
    {
        return m_buffer;
    }

    char* BeginWrite()
    {
        return Begin() + m_writerIndex;
    }

    char* BeginRead()
    {
        return Begin() + m_readerIndex;
    }

	// apply more space for saving datas
	void MakeSpace(int len)
	{
		// 1. 当剩余的空间不足以存放新添加进来的数据，则扩展内存空间
		if (len > WritableBytes())
		{
			// 1.1 以当前的容量为参考，需要再扩展几倍的空间
			int multiple = len / m_capacity;
			if (len % m_capacity > 0)
				multiple += 1;
			// 1.2 如果需要大于固定倍数（FixMultiple）才能满足新数据的存放需要，
			// 则使用1.1的值，反之，使用固定倍数扩展空间
			int finalMultiple = multiple > FixMultiple ? multiple : FixMultiple;
			// 1.3 扩展空间
			m_buffer = (char*)realloc(m_buffer, m_capacity * (finalMultiple + 1));
			if (m_buffer == NULL) {
				printf("Realloc failed. Not Enough Memory!\n");
				return;
			}
			// 1.4 初始化新扩展的空间
			memset(m_buffer + m_capacity, 0, m_capacity * finalMultiple);
			// 1.5 更新存放数据的容量
			m_capacity = m_capacity * (finalMultiple + 1);
		}
	}

    void EnsureWritableBytes(int len)
    {
        if (WritableBytes() < len) {
            MakeSpace(len);
        }
        assert(WritableBytes() >= len);
    }

	// 预留空间
    int ReserveSpace(int len)
    {
        EnsureWritableBytes(len);
        int start = m_writerIndex;
        Advance(len);
        return start;
    }

    void Append(const char* data, int len)
    {
        EnsureWritableBytes(len);
        memcpy(BeginWrite(), data, len);
        Advance(len);
    }

    void Append(const void* data, int len)
    {
        Append(static_cast<const char*>(data), len);
    }

	void Append(char* target, const void* src, int len)
	{
		memcpy(target, src, len);
	}

    void Append(char value)
    {
        Append(&value, sizeof(value));
    }

    void Append(int value)
    {
        //_toIntEndian(value);
        Append(&value, sizeof(value));
    }

    //void Write(IMIX_FAST_BOOLEAN value)
    //{
    //    Write(static_cast<int>(value));
    //}

    void Append(long value)
    {
        //_toLongEndian(value);
        Append(&value, sizeof(value));
    }

    void Append(double value)
    {
        Append(&value, sizeof(value));
    }

    void Append(const std::string& value)
    {
        Append(value.c_str(), value.size());
    }

    void Read(char* value, int len)
    {
        assert(ReadableBytes() >= len);
        memcpy(value, BeginRead(), len);
        Retrieve(len);
    }

    void Read(int* value, int len)
    {
        assert(ReadableBytes() >= len);
        memcpy(value, BeginRead(), len);
        //*value = _getIntEndian(*value);
        Retrieve(len);
    }

    //void Read(IMIX_FAST_BOOLEAN* value, int len)
    //{
    //    int val = 0;
    //    Read(&val, len);
    //    *value = static_cast<IMIX_FAST_BOOLEAN>(val);
    //}

    void Read(long* value, int len)
    {
        assert(ReadableBytes() >= len);
        memcpy(value, BeginRead(), len);
        //*value = _getLongEndian(*value);
        Retrieve(len);
    }

    void Read(double* value, int len)
    {
        assert(ReadableBytes() >= len);
        memcpy(value, BeginRead(), len);
        // *value = _getDoubleEndian(*value);
        Retrieve(len);
    }

    void Read(std::string* value, int len)
    {
        assert(ReadableBytes() >= len);
        value->clear();
        value->append(BeginRead(), len);
        Retrieve(len);
    }

private:
    void Advance(int amount)
    {
        m_writerIndex += amount;
    }

    void Retrieve(int len)
    {
        assert(len <= ReadableBytes());
        if (len < ReadableBytes())
		{
            m_readerIndex += len;
        }
		else
		{
            m_readerIndex = 0;
            m_writerIndex = 0;
        }
    }

private:
	const int FixMultiple;
    char* m_buffer;
    int m_readerIndex;
    int m_writerIndex;
    int m_capacity;
};
