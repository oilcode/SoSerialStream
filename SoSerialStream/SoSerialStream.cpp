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
// 5，本类还没有实现压缩功能。也没有加密。
//-----------------------------------------------------------------------------
#include <Windows.h>
#include "SoSerialStream.h"
//-----------------------------------------------------------------------------
namespace GGUI
{
	SoSerialStream::SoSerialStream()
	:m_pBuffer(0)
	,m_nSize(0)
	,m_nCursorPos(0)
	,m_eLastOpeResult(OpeResult_OK)
	,m_eMode(Mode_None)
	{
		m_pBuffer = (char*)malloc(SoSerialStream_BufferSize);
	}
	//-----------------------------------------------------------------------------
	SoSerialStream::~SoSerialStream()
	{
		if (m_pBuffer)
		{
			free(m_pBuffer);
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
	void SoSerialStream::Fill(const char* pBuffer, __int32 nValidSize)
	{
		memcpy(m_pBuffer, pBuffer, nValidSize);
		m_nSize = nValidSize;
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
} //namespace GGUI
//-----------------------------------------------------------------------------
