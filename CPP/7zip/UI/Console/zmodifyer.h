#pragma once

#include "StdAfx.h"

#if defined( _WIN32) && defined( _7ZIP_LARGE_PAGES)
#include "../../../../C/Alloc.h"
#endif

#include "Common/MyInitGuid.h"

#include "Common/CommandLineParser.h"
#include "Common/IntToString.h"
#include "Common/MyException.h"
#include "Common/StdOutStream.h"
#include "Common/StringConvert.h"
#include "Common/StringToInt.h"

#include "Windows/Error.h"
#ifdef _WIN32
#include "Windows/MemoryLock.h"
#endif

#include "../Common/ArchiveCommandLine.h"
#include "../Common/ExitCode.h"
#include "../Common/Extract.h"
#ifdef EXTERNAL_CODECS
#include "../Common/LoadCodecs.h"
#endif
#include "../Common/PropIDUtils.h"

#include "BenchCon.h"
#include "ExtractCallbackConsole.h"
#include "List.h"
#include "OpenCallbackConsole.h"
#include "UpdateCallbackConsole.h"

#include "../../MyVersion.h"

#include "../../Archive/7z/7zDB.h"

#include <string>

class ZDB;

class zmodifyer
{
public:

	// 최초 한번만 실행
	static void init();

	zmodifyer(wchar_t const * file_name);

	~zmodifyer();

	// file_name.empty() 이면 모든 파일 압축해제
	bool extract(UStringVector & file_names, wchar_t const * password = 0, wchar_t const * outDir = 0, bool allPath = true);

	// 파일 추가 또는 업데이트
	void add( UStringVector & file_names, int level = 5, wchar_t const * password = 0 );

	// 파일 제거
	void del( UStringVector & file_names, wchar_t const * password = 0 );

	// 스트림 읽기
	unsigned char * get( UString & file_name, size_t& size, wchar_t const * password = 0 );

	// 폴더 정보 읽기
	void getInfo( std::vector< std::pair< std::wstring, ZFile* > > & fileList, wchar_t const * path, wchar_t const * password = 0 );

	// 버퍼 해제
	void release( Byte * p );

	void clearDB();

	static bool compress( unsigned char const * src, size_t srcLen, unsigned char * dest, size_t & destLen, int level = 5 );
	static bool uncompress(unsigned char *dest, size_t & destLen, const unsigned char *src, size_t  srcLen);

private:
	CCodecs *codecs_; // 압축 코덱
	UString file_name_; // 압축 파일 이름
	Byte key[16];
	
	CStdOutStream outStream_; // 출력 스트림
	ZDB * zdb_;



	// 커맨드라인 정보로 options 셋팅
	void OptionSetting(UStringVector& commandStrings, CArchiveCommandLineOptions & options);
	bool update( UStringVector & file_names,UStringVector & command, wchar_t const * password = 0 );
};