//-----------------------------------------------------------------------------
// SoSerialStream
// (C) oil
// 2013-07-25
// ��д�ڴ�����ʱ�д��С�˵����⣬��δ�����
//-----------------------------------------------------------------------------
#ifndef _SoSerialStream_h_
#define _SoSerialStream_h_
//-----------------------------------------------------------------------------
//��������С��10M��
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
			OpeResult_WrongMode, //��дģʽ��ƥ�䡣
			OpeResult_Write_NotEnoughBuffer, //ʣ�໺�������㣬д��ʧ�ܡ�
			OpeResult_Read_NotEnoughBuffer, //ʣ�໺�������㣬��ȡʧ�ܡ�
			OpeResult_Read_StringNoZero, //��ȡһ���ַ���ʱ��û���ҵ���������
		};
		struct stStringForWrite
		{
			const char* szString; //������utf8�ַ�����
			__int32 nLength; //�ַ������ȣ���λByte����������������
		};
		struct stStringForRead
		{
			//ע�⣬szString��ָ����ַ��������ڴ���λ�ڱ����m_pBuffer�У�
			//���Ҫ���ϰ�����ַ����������Լ����ڴ��
			char* szString; //������utf8�ַ�����
			__int32 nLength; //�ַ������ȣ���λByte����������������
		};
	public:
		SoSerialStream();
		~SoSerialStream();

		//д������
		SoSerialStream& operator << (bool bValue);
		SoSerialStream& operator << (char cValue); //char��Ϊ__int8
		SoSerialStream& operator << (__int16 int16Value);
		SoSerialStream& operator << (__int32 int32Value);
		SoSerialStream& operator << (__int64 int64Value);
		SoSerialStream& operator << (const stStringForWrite& szString);
		//��������
		SoSerialStream& operator >> (bool& bValue);
		SoSerialStream& operator >> (char& cValue);
		SoSerialStream& operator >> (__int16& int16Value);
		SoSerialStream& operator >> (__int32& int32Value);
		SoSerialStream& operator >> (__int64& int64Value);
		SoSerialStream& operator >> (stStringForRead& szString);

		__int32 GetSize() const;
		eOperationResult GetLastOpeResult() const;
		const char* GetBuffer() const;
		void Clear();
		void ClearForWrite();
		void ClearForRead();
		void Fill(const char* pBuffer, __int32 nValidSize);

	private:
		//��������
		char* m_pBuffer;
		//��Ч���ȣ���λByte��
		__int32 m_nSize;
		//��ǰ���λ�á���0��ʼ��������������д�����ӹ�괦��ʼ��
		__int32 m_nCursorPos;
		//�����һ��ִ�ж�д�����Ľ����
		eOperationResult m_eLastOpeResult;
		//��дģʽ��
		eOperationMode m_eMode;
	};
	//-----------------------------------------------------------------------------
	inline __int32 SoSerialStream::GetSize() const
	{
		return m_nSize;
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
	inline void SoSerialStream::Clear()
	{
		m_nSize = 0;
		m_nCursorPos = 0;
		m_eLastOpeResult = OpeResult_OK;
		m_eMode = Mode_None;
	}
	//-----------------------------------------------------------------------------
	inline void SoSerialStream::ClearForWrite()
	{
		m_nSize = 0;
		m_nCursorPos = 0;
		m_eLastOpeResult = OpeResult_OK;
		m_eMode = Mode_Write;
	}
	//-----------------------------------------------------------------------------
	inline void SoSerialStream::ClearForRead()
	{
		m_nSize = 0;
		m_nCursorPos = 0;
		m_eLastOpeResult = OpeResult_OK;
		m_eMode = Mode_Read;
	}
}
//-----------------------------------------------------------------------------
#endif //_SoSerialStream_h_
//-----------------------------------------------------------------------------
