//-----------------------------------------------------------------------------
// SoSerialStream
// (C) oil
// 2013-07-25
//-----------------------------------------------------------------------------
#ifndef _SoSerialStream_h_
#define _SoSerialStream_h_
//-----------------------------------------------------------------------------
//缓存区大小，10M。
#define SoSerialStream_BufferSize 10240000
//-----------------------------------------------------------------------------
namespace GGUI
{
	class SoSerialStream
	{
	public:
		enum eOperationMode
		{
			Mode_None,
			Mode_Write,
			Mode_Read,
		};
		enum eOperationResult
		{
			OpeResult_OK = 0,
			OpeResult_WrongMode, //读写模式不匹配。
			OpeResult_Write_NotEnoughBuffer, //剩余缓存区不足，写入失败。
			OpeResult_Read_NotEnoughBuffer, //剩余缓存区不足，读取失败。
			OpeResult_Read_StringNoZero, //读取一个字符串时，没有找到结束符。
		};
		struct stStringForWrite
		{
			//如果utf8String值为NULL，则当做空字符串做处理。
			const char* utf8String; //必须是utf8字符串。
			__int32 nLength; //字符串长度，单位Byte，不包括结束符。
		};
		struct stStringForRead
		{
			//注意，szString所指向的字符串，其内存是位于本类的m_pBuffer中，
			//外界要马上把这个字符串拷贝到自己的内存里。
			char* utf8String; //必须是utf8字符串。
			__int32 nLength; //字符串长度，单位Byte，不包括结束符。
		};
	public:
		SoSerialStream();
		~SoSerialStream();

		//写操作。
		SoSerialStream& operator << (bool bValue);
		SoSerialStream& operator << (char cValue); //char即为__int8
		SoSerialStream& operator << (__int16 int16Value);
		SoSerialStream& operator << (__int32 int32Value);
		SoSerialStream& operator << (__int64 int64Value);
		SoSerialStream& operator << (const stStringForWrite& szString);
		//读操作。
		SoSerialStream& operator >> (bool& bValue);
		SoSerialStream& operator >> (char& cValue);
		SoSerialStream& operator >> (__int16& int16Value);
		SoSerialStream& operator >> (__int32& int32Value);
		SoSerialStream& operator >> (__int64& int64Value);
		SoSerialStream& operator >> (stStringForRead& szString);

		__int32 GetSize() const;
		__int32 GetSizeAfterCompress() const;
		eOperationResult GetLastOpeResult() const;
		const char* GetBuffer() const;
		const char* GetBufferAfterCompress() const;
		void Clear();
		void ClearForWrite();
		void ClearForRead();
		//--pBuffer 是经过压缩后的数据。
		void FillDataForRead(const char* pBuffer, __int32 nValidSize);
		__int64 hton64(__int64 theValue);
		__int64 ntoh64(__int64 theValue);
		//压缩与解压缩
		void Compress();
		void Uncompress();

	private:
		//本机字节序与网络字节序互相转换时，用于转换64位整数。
		struct stConvert64
		{
			__int32 a;
			__int32 b;
		};

	private:
		//缓存区。
		char* m_pBuffer;
		//用于执行压缩和解压缩的临时缓存区。
		char* m_pBufferAfterCompress;
		//有效长度，单位Byte。
		__int32 m_nSize;
		//压缩后的长度。
		__int32 m_nSizeAfterCompress;
		//当前光标位置。从0开始计数。读操作和写操作从光标处开始。
		__int32 m_nCursorPos;
		//最近的一次执行读写操作的结果。
		eOperationResult m_eLastOpeResult;
		//读写模式。
		eOperationMode m_eMode;
	};
	//-----------------------------------------------------------------------------
	inline __int32 SoSerialStream::GetSize() const
	{
		return m_nSize;
	}
	//-----------------------------------------------------------------------------
	inline __int32 SoSerialStream::GetSizeAfterCompress() const
	{
		return m_nSizeAfterCompress;
	}
	//-----------------------------------------------------------------------------
	inline SoSerialStream::eOperationResult SoSerialStream::GetLastOpeResult() const
	{
		return m_eLastOpeResult;
	}
	//-----------------------------------------------------------------------------
	inline const char* SoSerialStream::GetBuffer() const
	{
		return m_pBuffer;
	}
	//-----------------------------------------------------------------------------
	inline const char* SoSerialStream::GetBufferAfterCompress() const
	{
		return m_pBufferAfterCompress;
	}
	//-----------------------------------------------------------------------------
	inline void SoSerialStream::Clear()
	{
		m_nSize = 0;
		m_nSizeAfterCompress = 0;
		m_nCursorPos = 0;
		m_eLastOpeResult = OpeResult_OK;
		m_eMode = Mode_None;
	}
	//-----------------------------------------------------------------------------
	inline void SoSerialStream::ClearForWrite()
	{
		m_nSize = 0;
		m_nSizeAfterCompress = 0;
		m_nCursorPos = 0;
		m_eLastOpeResult = OpeResult_OK;
		m_eMode = Mode_Write;
	}
	//-----------------------------------------------------------------------------
	inline void SoSerialStream::ClearForRead()
	{
		m_nSize = 0;
		m_nSizeAfterCompress = 0;
		m_nCursorPos = 0;
		m_eLastOpeResult = OpeResult_OK;
		m_eMode = Mode_Read;
	}
}
//-----------------------------------------------------------------------------
#endif //_SoSerialStream_h_
//-----------------------------------------------------------------------------
