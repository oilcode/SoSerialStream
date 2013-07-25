//-----------------------------------------------------------------------------
// SoSerialStream
// (C) oil
// 2013-07-25
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
		memcpy(m_pBuffer+m_nCursorPos, &int16Value, 2);
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
		memcpy(m_pBuffer+m_nCursorPos, &int32Value, 4);
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
		memcpy(m_pBuffer+m_nCursorPos, &int64Value, 8);
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
		if (szString.szString == 0 || szString.nLength == 0)
		{
			m_pBuffer[m_nCursorPos] = 0;
			m_nCursorPos += 1;
		}
		else
		{
			memcpy(m_pBuffer+m_nCursorPos, szString.szString, szString.nLength+1);
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
		memcpy(&int16Value, m_pBuffer+m_nCursorPos, 2);
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
		memcpy(&int32Value, m_pBuffer+m_nCursorPos, 4);
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
		memcpy(&int64Value, m_pBuffer+m_nCursorPos, 8);
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
			szString.szString = m_pBuffer + nLastCursorPos;
			m_nCursorPos += szString.nLength + 1; //包括结束符。
			m_eLastOpeResult = OpeResult_OK;
			return *this;
		}
		else
		{
			szString.szString = 0;
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
} //namespace GGUI
//-----------------------------------------------------------------------------
