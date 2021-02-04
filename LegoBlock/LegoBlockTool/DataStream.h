
#pragma once

#include <windows.h>

#define DI8_MAX				(DI8)0xFF
#define DI16_MAX				(DI16)0xFFFF
#define DI32_MAX				(DI32)0xFFFFFFFF
#define DI64_MAX				(DI64)0xFFFFFFFFFFFFFFFF
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	typedef unsigned __int8		DI8;
	typedef unsigned __int16		DI16;
	typedef unsigned __int32		DI32;
	typedef unsigned __int64		DI64;
	typedef float						DF32;
	typedef double					DF64;
	typedef long double			DF80;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class CDataStream
	{
    public:
		CDataStream(void);
		CDataStream(char* fn);
		~CDataStream(void);

	private:
		char file_name[256];
		HANDLE hFile;
		DI16 err;
		DI32 file_size;
		DI8* buf_ptr;
		DI8* buf_cur_ptr;
		DI8* buf_last_ptr;
		BOOL bDMA;
		BOOL	 bOpenToRead;
		BOOL	 bOpenToReadStream;

	public:
		void Init(char *fname);
		void InitDirectMemoryAccess(char* buf, DWORD speclen);
		DWORD MemoryToFile(DWORD len);
		BOOL OpenToReadStream(void);
		BOOL OpenToRead();
        BOOL OpenToWrite(void);
		BOOL CreateToWrite(void);
		void Close(void);


		template<typename T>
		DWORD GetData(T* v, int len = 1) {
			if(bOpenToReadStream) {
				DWORD readbytes = 0;
				ReadFile(hFile, v, sizeof(T) * len, &readbytes, NULL);
				return readbytes;
			}

			for(int i=0; i<len; i++) {
				if(buf_cur_ptr > buf_last_ptr /*&& !bDMA*/) {
					err = 1;
					return 0;
				}
				
				*(v+i) = *(T*)buf_cur_ptr;
				buf_cur_ptr += sizeof(T);
			}
			
			return sizeof(T)*len;
		}

		DWORD GetBlock(void* v, int len);

		template<typename T>
		void PutData(T* v, int len = 1);
		void PutBlock(void* v, int len);

		void Seek(int pos);
		void Current(int* pos);
		void Skip(int pos);
		void SkipBack(int pos);
		void Rewind(void);

		template<typename T>
		T* GetRawCurPtr(void) {
			if(FALSE == bOpenToRead)	
				return NULL;

			if(buf_cur_ptr > buf_last_ptr /*&& !bDMA*/)
				return NULL;
			
			return (T*)buf_cur_ptr;
		}

		template<typename T>
		T* GetRawBufferPtr(void)	{ return (T*)buf_ptr; }

		DI16 Err(void) { return err; }
		DI32 GetSize(void) { return file_size; }
		BOOL IsEnd(void);
		DWORD GetWrittenBytes(void) { return (DWORD)(buf_cur_ptr - buf_ptr); }
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////