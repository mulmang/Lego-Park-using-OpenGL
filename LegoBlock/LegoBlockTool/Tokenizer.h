#pragma once

#include "DataStream.h"

#define MAX_ITEM		50
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class CTokenizer
	{
	public:
		CTokenizer(void);
		CTokenizer(char* filename);
		~CTokenizer(void);

	public:
		CDataStream ds;
		LONG fs;

		char buff[256];
		char group_maker_list[MAX_ITEM][2];
		int group_maker_list_cnt;
		char separator_list[MAX_ITEM];
		int separator_list_cnt;
		char delimiter_list[MAX_ITEM];
		int delimiter_list_cnt;
		char ignore_list[MAX_ITEM];
		int ignore_list_cnt;
		char null_list[MAX_ITEM];
		int null_list_cnt;

		BOOL b_groupmaker;
		BOOL b_separator;
		BOOL b_delimiter;

		void (*callbackfn)(int, char c, void*);
		void *cbparam;

	public:
		char* GetNext(int* len);
		char* PeekNext(int* len);
		char* PeekPtr(void);

		void Init(void);
		void SetLine(char* ptr, int len);
		int	TestTokenChar(char c, BOOL b_cb);

		void ClearGroupMakerKey(void);
		void ClearSeparatorKey(void);
		void ClearDelimiterKey(void);
		BOOL AddGroupMakerKey(char k, char k_sym);
		char GetGroupSymmetryKey(char k);
		BOOL AddSeparatorKey(char ch);
		BOOL AddDelimiterKey(char ch);
		void SetKeyActivation(int type, BOOL b_state);
		void LoadDefaultKey(void);
		void ClearIgnoreKey(void);
		BOOL AddIgnoreKey(char ch);
		BOOL AddNullKey(char ch);
		void ClearNullKey(void);
		void SetCallback(void (*_cb)(int, char c, void*), void* param);
	};
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////