#include "SoSerialStream.h"
#include <string.h>
using namespace GGUI;

void main()
{
	SoSerialStream kStreamForWrite;
	kStreamForWrite.ClearForWrite();
	kStreamForWrite << false;
	kStreamForWrite << (char)44;
	kStreamForWrite << (__int64)3334444;
	kStreamForWrite << (__int16)22333;
	SoSerialStream::stStringForWrite kWriteString;
	const char* pszStringForWrite = "JianSheWoMenDeJiaYuan";
	kWriteString.utf8String = pszStringForWrite;
	kWriteString.nLength = strlen(pszStringForWrite);
	kStreamForWrite << kWriteString;
	kWriteString.utf8String = "";
	kWriteString.nLength = 0;
	kStreamForWrite << kWriteString;
	kStreamForWrite << (__int32)4444;
	kWriteString.utf8String = pszStringForWrite;
	kWriteString.nLength = strlen(pszStringForWrite);
	kStreamForWrite << kWriteString;

	SoSerialStream kStreamForRead;
	kStreamForRead.ClearForRead();
	kStreamForRead.Fill(kStreamForWrite.GetBuffer(), kStreamForWrite.GetSize());
	bool bValue;
	char cValue;
	__int64 int64Value;
	__int16 int16Value;
	__int32 int32Value;
	SoSerialStream::stStringForRead kReadString;
	kStreamForRead >> bValue;
	kStreamForRead >> cValue;
	kStreamForRead >> int64Value;
	kStreamForRead >> int16Value;
	kStreamForRead >> kReadString;
	kStreamForRead >> kReadString;
	kStreamForRead >> int32Value;
	kStreamForRead >> kReadString;

	kStreamForRead << 0;

}