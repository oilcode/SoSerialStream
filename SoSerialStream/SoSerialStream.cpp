//-----------------------------------------------------------------------------
// SoSerialStream
// (C) oil
// 2013-07-25
//
// 1，已经对本机字节序和网络字节序做了相互转换。本类内部实现了64位整数的相互转换。
// 2，字节序转换函数使用了winsock函数，所以要依赖Ws2_32.lib。
// 3，写字符串和读字符串时，字符串都是UTF8格式。
// 4，读字符串后，stStringForRead::utf8String指向的字符串内存是位于本类的m_pBuffer中，
//    外界要马上把这个字符串拷贝到自己的内存里。
// 5，加入了压缩功能，使用ZLIB。
//-----------------------------------------------------------------------------
#include <Windows.h>
#include "SoSerialStream.h"
#define ZLIB_WINAPI
#include "zlib.h"
//-----------------------------------------------------------------------------
namespace GGUI
{
	SoSerialStream::SoSerialStream()
	:m_pBuffer(0)
	,m_pBufferAfterCompress(0)
	,m_nSize(0)
	,m_nSizeAfterCompress(0)
	,m_nCursorPos(0)
	,m_eLastOpeResult(OpeResult_OK)
	,m_eMode(Mode_None)
	{
		m_pBuffer = (char*)malloc(SoSerialStream_BufferSize);
		m_pBufferAfterCompress = (char*)malloc(SoSerialStream_BufferSize);
	}
	//-----------------------------------------------------------------------------
	SoSerialStream::~SoSerialStream()
	{
		if (m_pBuffer)
		{
			free(m_pBuffer);
		}
		if (m_pBufferAfterCompress)
		{
			free(m_pBufferAfterCompress);
		}
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator << (bool bValue)
	{
		if (m_eMode != Mode_Write)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos + 1 > SoSerialStream_BufferSize)
		{
			m_eLastOpeResult = OpeResult_Write_NotEnoughBuffer;
			return *this;
		}
		m_pBuffer[m_nCursorPos] = bValue ? 1 : 0;
		m_nCursorPos += 1;
		if (m_nSize < m_nCursorPos)
		{
			m_nSize = m_nCursorPos;
		}
		m_eLastOpeResult = OpeResult_OK;
		return *this;
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator << (char cValue)
	{
		if (m_eMode != Mode_Write)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos + 1 > SoSerialStream_BufferSize)
		{
			m_eLastOpeResult = OpeResult_Write_NotEnoughBuffer;
			return *this;
		}
		m_pBuffer[m_nCursorPos] = cValue;
		m_nCursorPos += 1;
		if (m_nSize < m_nCursorPos)
		{
			m_nSize = m_nCursorPos;
		}
		m_eLastOpeResult = OpeResult_OK;
		return *this;
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator << (__int16 int16Value)
	{
		if (m_eMode != Mode_Write)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos + 2 > SoSerialStream_BufferSize)
		{
			m_eLastOpeResult = OpeResult_Write_NotEnoughBuffer;
			return *this;
		}
		__int16 convertedValue = htons(int16Value);
		memcpy(m_pBuffer+m_nCursorPos, &convertedValue, 2);
		m_nCursorPos += 2;
		if (m_nSize < m_nCursorPos)
		{
			m_nSize = m_nCursorPos;
		}
		m_eLastOpeResult = OpeResult_OK;
		return *this;
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator << (__int32 int32Value)
	{
		if (m_eMode != Mode_Write)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos + 4 > SoSerialStream_BufferSize)
		{
			m_eLastOpeResult = OpeResult_Write_NotEnoughBuffer;
			return *this;
		}
		__int32 convertedValue = htonl(int32Value);
		memcpy(m_pBuffer+m_nCursorPos, &convertedValue, 4);
		m_nCursorPos += 4;
		if (m_nSize < m_nCursorPos)
		{
			m_nSize = m_nCursorPos;
		}
		m_eLastOpeResult = OpeResult_OK;
		return *this;
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator << (__int64 int64Value)
	{
		if (m_eMode != Mode_Write)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos + 8 > SoSerialStream_BufferSize)
		{
			m_eLastOpeResult = OpeResult_Write_NotEnoughBuffer;
			return *this;
		}
		__int64 convertedValue = hton64(int64Value);
		memcpy(m_pBuffer+m_nCursorPos, &convertedValue, 8);
		m_nCursorPos += 8;
		if (m_nSize < m_nCursorPos)
		{
			m_nSize = m_nCursorPos;
		}
		m_eLastOpeResult = OpeResult_OK;
		return *this;
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator << (const stStringForWrite& szString)
	{
		if (m_eMode != Mode_Write)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos + szString.nLength + 1 > SoSerialStream_BufferSize)
		{
			m_eLastOpeResult = OpeResult_Write_NotEnoughBuffer;
			return *this;
		}
		if (szString.utf8String == 0 || szString.nLength == 0)
		{
			m_pBuffer[m_nCursorPos] = 0;
			m_nCursorPos += 1;
		}
		else
		{
			memcpy(m_pBuffer+m_nCursorPos, szString.utf8String, szString.nLength+1);
			m_nCursorPos += szString.nLength + 1;
		}
		if (m_nSize < m_nCursorPos)
		{
			m_nSize = m_nCursorPos;
		}
		m_eLastOpeResult = OpeResult_OK;
		return *this;
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator >> (bool& bValue)
	{
		if (m_eMode != Mode_Read)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos + 1 > m_nSize)
		{
			m_eLastOpeResult = OpeResult_Read_NotEnoughBuffer;
			return *this;
		}
		bValue = (m_pBuffer[m_nCursorPos] == 1);
		m_nCursorPos += 1;
		m_eLastOpeResult = OpeResult_OK;
		return *this;
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator >> (char& cValue)
	{
		if (m_eMode != Mode_Read)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos + 1 > m_nSize)
		{
			m_eLastOpeResult = OpeResult_Read_NotEnoughBuffer;
			return *this;
		}
		cValue = m_pBuffer[m_nCursorPos];
		m_nCursorPos += 1;
		m_eLastOpeResult = OpeResult_OK;
		return *this;
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator >> (__int16& int16Value)
	{
		if (m_eMode != Mode_Read)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos + 2 > m_nSize)
		{
			m_eLastOpeResult = OpeResult_Read_NotEnoughBuffer;
			return *this;
		}
		__int16 srcValue = 0;
		memcpy(&srcValue, m_pBuffer+m_nCursorPos, 2);
		int16Value = ntohs(srcValue);
		m_nCursorPos += 2;
		m_eLastOpeResult = OpeResult_OK;
		return *this;
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator >> (__int32& int32Value)
	{
		if (m_eMode != Mode_Read)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos + 4 > m_nSize)
		{
			m_eLastOpeResult = OpeResult_Read_NotEnoughBuffer;
			return *this;
		}
		__int32 srcValue = 0;
		memcpy(&srcValue, m_pBuffer+m_nCursorPos, 4);
		int32Value = ntohl(srcValue);
		m_nCursorPos += 4;
		m_eLastOpeResult = OpeResult_OK;
		return *this;
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator >> (__int64& int64Value)
	{
		if (m_eMode != Mode_Read)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos + 8 > m_nSize)
		{
			m_eLastOpeResult = OpeResult_Read_NotEnoughBuffer;
			return *this;
		}
		__int64 srcValue = 0;
		memcpy(&srcValue, m_pBuffer+m_nCursorPos, 8);
		int64Value = ntoh64(srcValue);
		m_nCursorPos += 8;
		m_eLastOpeResult = OpeResult_OK;
		return *this;
	}
	//-----------------------------------------------------------------------------
	SoSerialStream& SoSerialStream::operator >> (stStringForRead& szString)
	{
		if (m_eMode != Mode_Read)
		{
			m_eLastOpeResult = OpeResult_WrongMode;
			return *this;
		}
		if (m_nCursorPos >= m_nSize)
		{
			m_eLastOpeResult = OpeResult_Read_NotEnoughBuffer;
			return *this;
		}
		bool bFindZero = false;
		const __int32 nLastCursorPos = m_nCursorPos;
		for (__int32 i=nLastCursorPos; i<m_nSize; ++i)
		{

			if (m_pBuffer[i] == 0)
			{
				szString.nLength = i - nLastCursorPos; //不含结束符。
				bFindZero = true;
				break;
			}
		}
		if (bFindZero)
		{
			szString.utf8String = m_pBuffer + nLastCursorPos;
			m_nCursorPos += szString.nLength + 1; //包括结束符。
			m_eLastOpeResult = OpeResult_OK;
			return *this;
		}
		else
		{
			szString.utf8String = 0;
			szString.nLength = 0;
			m_eLastOpeResult = OpeResult_Read_StringNoZero;
			return *this;
		}
	}
	//-----------------------------------------------------------------------------
	void SoSerialStream::FillDataForRead(const char* pBuffer, __int32 nValidSize)
	{
		memcpy(m_pBufferAfterCompress, pBuffer, nValidSize);
		m_nSizeAfterCompress = nValidSize;
	}
	//-----------------------------------------------------------------------------
	__int64 SoSerialStream::hton64(__int64 theValue)
	{
		__int64 theResult = 0;
		stConvert64* pSrcValue = (stConvert64*)(&theValue);
		stConvert64* pDestValue = (stConvert64*)(&theResult);
		pDestValue->a = htonl(pSrcValue->a);
		pDestValue->b = htonl(pSrcValue->b);
		return theResult;
	}
	//-----------------------------------------------------------------------------
	__int64 SoSerialStream::ntoh64(__int64 theValue)
	{
		__int64 theResult = 0;
		stConvert64* pSrcValue = (stConvert64*)(&theValue);
		stConvert64* pDestValue = (stConvert64*)(&theResult);
		pDestValue->a = ntohl(pSrcValue->a);
		pDestValue->b = ntohl(pSrcValue->b);
		return theResult;
	}
	//-----------------------------------------------------------------------------
	void SoSerialStream::Compress()
	{
		m_nSizeAfterCompress = -1; //置为无效值。
		uLongf nSizeAfterCompress = SoSerialStream_BufferSize;
		int nResult = compress((Bytef*)m_pBufferAfterCompress, &nSizeAfterCompress, (Bytef*)m_pBuffer, m_nSize);
		if (nResult == Z_OK)
		{
			//成功。
			m_nSizeAfterCompress = (__int32)nSizeAfterCompress;
		}
		else if (nResult == Z_BUF_ERROR)
		{
			//输出缓存区不够大。
			//未做处理。
		}
		else if (nResult == Z_MEM_ERROR)
		{
			//内存不足。
			//未做处理。
		}
	}
	//-----------------------------------------------------------------------------
	void SoSerialStream::Uncompress()
	{
		m_nSize = -1; //置为无效值。
		uLongf nSizeAfterUncompress = SoSerialStream_BufferSize;
		int nResult = uncompress((Bytef*)m_pBuffer, &nSizeAfterUncompress, (Bytef*)m_pBufferAfterCompress, m_nSizeAfterCompress);
		if (nResult == Z_OK)
		{
			//成功。
			m_nSize = (__int32)nSizeAfterUncompress;
		}
		else if (nResult == Z_BUF_ERROR)
		{
			//输出缓存区不够大。
			//未做处理。
		}
		else if (nResult == Z_DATA_ERROR)
		{
			//输入的数据有误。
			//未做处理。
		}
		else if (nResult == Z_MEM_ERROR)
		{
			//内存不足。
			//未做处理。
		}
	}
} //namespace GGUI
//-----------------------------------------------------------------------------
