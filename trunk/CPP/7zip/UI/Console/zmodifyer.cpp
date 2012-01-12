#include "zmodifyer.h"
#include <vector>
#include <algorithm>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <list>
#include "../../../C/LzmaEnc.h"
#include "../../../C/LzmaDec.h"
#include "utill_include.h"



// ---------------------------
// exception messages

#define kEverythingIsOk "Everything is Ok"
#define kUserErrorMessage "Incorrect command line"
#define kNoFormats "7-Zip cannot find the code that works with archives."
#define kUnsupportedArcTypeMessage "Unsupported archive type"
#define kDefaultSfxModule L"7zCon.sfx"

// 최초 한번만 실행
void zmodifyer::init() {

	static bool first = true;

	if( !first )
		return;

	first = false;

	//g_StdOut.SetStream( NULL );

#if defined(_WIN32) && !defined(UNDER_CE)
	SetFileApisToOEM(); // ??
#endif
}

zmodifyer::zmodifyer(wchar_t const * file_name)
	: outStream_( 0 )
	, file_name_(file_name ? file_name : L"")
	, codecs_(0)
	, zdb_( new ZDB( file_name ? file_name : L"" ) )
{
	init();

	memset( key, 0, sizeof(key) );

	// 압축 코덱 생성
	codecs_ = new CCodecs;
	HRESULT result = codecs_->Load();
	if (result != S_OK)
	{
		delete codecs_;
		codecs_ = 0;
		//throw CSystemException(result);
		return;
	}

	#if defined(_WIN32) && defined(_7ZIP_LARGE_PAGES)
	if (options.LargePages)
	{
		SetLargePageSize();
		NSecurity::EnableLockMemoryPrivilege();
	}
	#endif
}

zmodifyer::~zmodifyer() {
	if( codecs_ )
		delete codecs_;

	if( zdb_ )
		delete zdb_;
}

void zmodifyer::getInfo( std::vector< std::pair< std::wstring, ZFile* > > & fileList, wchar_t const * path, wchar_t const * password )
{
	if( zdb_->db_.IsEmpty() )
	{
		// 커맨드 스트링
		UStringVector commandStrings;

		commandStrings.Add(L"L");

		UString pw( L"-P" );

		commandStrings.Add( (pw + ( password ? password : L"") ) );

		commandStrings.Add( file_name_ );

		CArchiveCommandLineOptions options;

		OptionSetting( commandStrings, options );

		// 압축파일 형식 인덱스 추출.
		CIntVector formatIndices;

		if (!codecs_->FindFormatForArchiveType(options.ArcType, formatIndices))
		{
			throw kUnsupportedArcTypeMessage;
		}

		UInt64 numErrors = 0;

		HRESULT result = ListArchives(
			codecs_,
			formatIndices,
			options.StdInMode,
			options.ArchivePathsSorted,
			options.ArchivePathsFullSorted,
			options.WildcardCensor.Pairs.Front().Head,
			options.EnableHeaders,
			options.TechMode,
#ifndef _NO_CRYPTO
			options.PasswordEnabled,
			options.Password,
#endif
			numErrors, zdb_);
	}

	ZFile * zfile = 0;

	std::wstring filePath = path ? path : L"";

	if( filePath.empty() )
	{
		zfile = &zdb_->folder_;
	}
	else
	{
		zfile = zdb_->folder_.find( filePath );
	}

	if( zfile )
	{
		zfile->getList( fileList, filePath );
	}
}

void zmodifyer::release( Byte * p )
{
	CIOStream::Release( p );
}

unsigned char * zmodifyer::get( UString & file_name, size_t & size, wchar_t const * password /* = 0 */ )
{
	if( file_name.Length() == 0 )
		return 0;

	// 커맨드 스트링
	UStringVector commandStrings;

	commandStrings.Add( L"G" );

	UString pw( L"-P" );

	commandStrings.Add( (pw + ( password ? password : L"") ) );
	
	// multi thread
	commandStrings.Add( L"-MMT=+" );

	commandStrings.Add( L"-BD" );

	commandStrings.Add( file_name_ );

	commandStrings.Add( file_name );

	// 파싱된 커맨드라인 정보로 options 셋팅 ------------
	CArchiveCommandLineOptions options;
	
	//options.HelpMode = true;
	
	// 없는 파일이면 작업 중지
	if( !zdb_->db_.IsEmpty() )
	{
		typedef std::list< std::wstring > FileList;
		FileList files;

		std::wstring tmp = file_name;

		boost::split( files, tmp, boost::is_any_of(L":") );

		for( FileList::iterator iter = files.begin(); iter != files.end(); ++iter )
		{
			if( !zdb_->folder_.find( *iter ) )
				return 0;
		}
	}

	OptionSetting( commandStrings, options );
	options.OverwriteMode = NExtract::NOverwriteMode::kWithoutPrompt; // 덮어쓰기 할때 묻지마라.
	options.YesToAll = true;
	//------------

	// 압축파일 형식 인덱스 추출.
	CIntVector formatIndices;
	if (!codecs_->FindFormatForArchiveType(options.ArcType, formatIndices))
	{
		throw kUnsupportedArcTypeMessage;
	}

	bool isExtractGroupCommand = options.Command.IsFromExtractGroup();

	if (codecs_->Formats.Size() == 0 &&
		(isExtractGroupCommand ||
		options.Command.CommandType == NCommandType::kList ||
		options.Command.IsFromUpdateGroup()))
		throw kNoFormats;

	// 압축 해제
	if (isExtractGroupCommand)
	{
		CExtractCallbackConsole *ecs = new CExtractCallbackConsole;
		CMyComPtr<IFolderArchiveExtractCallback> extractCallback = ecs;

		ecs->OutStream = &outStream_;

#ifndef _NO_CRYPTO
		ecs->PasswordIsDefined = options.PasswordEnabled;
		ecs->Password = options.Password;
#endif

		ecs->Init();

		COpenCallbackConsole openCallback;
		openCallback.OutStream = &outStream_;

#ifndef _NO_CRYPTO
		openCallback.PasswordIsDefined = options.PasswordEnabled;
		openCallback.Password = options.Password;
#endif

		CExtractOptions eo;
		eo.StdInMode = options.StdInMode;
		eo.StdOutMode = options.StdOutMode;
		eo.PathMode = options.Command.GetPathMode();
		//eo.TestMode = options.Command.IsTestMode();
		eo.TestMode = true; // 테스트 모드로 압축해제(파일 저장 안함)
		eo.OverwriteMode = options.OverwriteMode;
		eo.OutputDir = options.OutputDir;
		eo.YesToAll = options.YesToAll;
		eo.CalcCrc = options.CalcCrc;

#if !defined(_7ZIP_ST) && !defined(_SFX)
		eo.Properties = options.ExtractProperties;
#endif

		//ZDBS::_currentArchive = file_name_;

		UString errorMessage;
		CDecompressStat stat;
		HRESULT result = DecompressArchives(
			codecs_,
			formatIndices,
			options.ArchivePathsSorted,
			options.ArchivePathsFullSorted,
			options.WildcardCensor.Pairs.Front().Head,
			eo, &openCallback, ecs, errorMessage, stat, zdb_);
	}

	return CIOStream::Read( size );
}

// file_name.empty() 이면 모든 파일 압축해제
bool zmodifyer::extract(UStringVector & file_names, wchar_t const * password, wchar_t const * outDir, bool allPath)
{
	// 커맨드 스트링
	UStringVector commandStrings;

	if( allPath )
		commandStrings.Add( L"X" );
	else
		commandStrings.Add( L"E" );

	UString pw( L"-P" );

	commandStrings.Add( (pw + ( password ? password : L"") ) );

	if( outDir )
	{
		UString od( L"-O" );
		commandStrings.Add( (od + outDir) );
	}
	commandStrings.Add( file_name_ );

	if( file_names.IsEmpty() )
	{
		commandStrings.Add( L"*" );
	}
	else
	{
		for( int i=0; i<file_names.Size(); ++i )
			commandStrings.Add( file_names[i] );
	}

	// 파싱된 커맨드라인 정보로 options 셋팅 ------------
	CArchiveCommandLineOptions options;
	OptionSetting( commandStrings, options );
	options.OverwriteMode = NExtract::NOverwriteMode::kWithoutPrompt; // 덮어쓰기 할때 묻지마라.
	options.YesToAll = true;
	//------------

	// 압축파일 형식 인덱스 추출.
	CIntVector formatIndices;
	if (!codecs_->FindFormatForArchiveType(options.ArcType, formatIndices))
	{
		//throw kUnsupportedArcTypeMessage;
		return false;
	}

	bool isExtractGroupCommand = options.Command.IsFromExtractGroup();

	if (codecs_->Formats.Size() == 0 &&
		(isExtractGroupCommand ||
		options.Command.CommandType == NCommandType::kList ||
		options.Command.IsFromUpdateGroup()))
		return false;
		//throw kNoFormats;

	// 압축 해제
	if (isExtractGroupCommand)
	{
		CExtractCallbackConsole *ecs = new CExtractCallbackConsole;
		CMyComPtr<IFolderArchiveExtractCallback> extractCallback = ecs;

		ecs->OutStream = &outStream_;

#ifndef _NO_CRYPTO
		ecs->PasswordIsDefined = options.PasswordEnabled;
		ecs->Password = options.Password;
#endif

		ecs->Init();

		COpenCallbackConsole openCallback;
		openCallback.OutStream = &outStream_;

#ifndef _NO_CRYPTO
		openCallback.PasswordIsDefined = options.PasswordEnabled;
		openCallback.Password = options.Password;
#endif

		CExtractOptions eo;
		eo.StdInMode = options.StdInMode;
		eo.StdOutMode = options.StdOutMode;
		eo.PathMode = options.Command.GetPathMode();
		eo.TestMode = options.Command.IsTestMode();
		eo.OverwriteMode = options.OverwriteMode;
		eo.OutputDir = options.OutputDir;
		eo.YesToAll = options.YesToAll;
		eo.CalcCrc = options.CalcCrc;

#if !defined(_7ZIP_ST) && !defined(_SFX)
		eo.Properties = options.ExtractProperties;
#endif
		//ZDBS::_currentArchive = file_name_;

		UString errorMessage;
		CDecompressStat stat;
		HRESULT result = DecompressArchives(
			codecs_,
			formatIndices,
			options.ArchivePathsSorted,
			options.ArchivePathsFullSorted,
			options.WildcardCensor.Pairs.Front().Head,
			eo, &openCallback, ecs, errorMessage, stat, zdb_);

		if( result == S_OK )
			return true;
	}

	return false;
}

void zmodifyer::add( UStringVector & file_names, int level, wchar_t const * password )
{
	if( level < 0 )
		level = 0;
	if( level > 9 )
		level = 9;

	// 커맨드 스트링
	UStringVector commandStrings;

	commandStrings.Add( L"U" );

	std::wstring compressLevel;

	std::wstringstream str;

	str << L"-MX" << level;

	str >> compressLevel;

	commandStrings.Add( compressLevel.c_str() );

	update( file_names, commandStrings, password );

	/*
	// 기존파일 업데이트를 시도하고 실패하면 추가
	if( !update( file_names, commandStrings, password ) )
	{
		commandStrings.Clear();

		commandStrings.Add( L"a" );

		commandStrings.Add( compressLevel.c_str() );

		update( file_names, commandStrings, password );
	}
	*/
}

void zmodifyer::del( UStringVector & file_names, wchar_t const * password )
{
	UStringVector commandStrings;

	commandStrings.Add( L"D" );

	update( file_names, commandStrings, password );
}

// 커맨드라인 정보로 options 셋팅
void zmodifyer::OptionSetting(UStringVector& commandStrings, CArchiveCommandLineOptions & options)
{		
	CArchiveCommandLineParser parser;

	parser.Parse1(commandStrings, options);
	
	//if (options.HelpMode)
	//{
		//ShowCopyrightAndHelp(g_StdOut, true);
		//return;
	//}

	parser.Parse2(options, zdb_);

	if( options.PasswordEnabled )
	{
		CMyComPtr< ICryptoSetPassword > setPassword = new CCryptoSetPassword;
		setPassword->CryptoSetPassword( (const Byte*)(&options.Password[0]), options.Password.Length() * 2 );
	}
}

bool zmodifyer::update( UStringVector & file_names, UStringVector & commandStrings, wchar_t const * password )
{
	if( file_names.IsEmpty() )
		return false;

	// 프로그래스 뷰 사용안함
	commandStrings.Add( L"-BD" );
	// 솔리드 압축 사용 안함
	commandStrings.Add( L"-MS=OFF" );
	// 압축 방식 lzma2
	commandStrings.Add( L"-M0=LZMA2" );
	// multi thread
	commandStrings.Add( L"-MMT=+" );

	if( password && wcslen(password) )
	{
		UString pw( L"-P" );
		commandStrings.Add( (pw + ( password ? password : L"") ) );
	}

	commandStrings.Add( file_name_ );

	for( int i=0; i<file_names.Size(); ++i )
		commandStrings.Add( file_names[i] );

	// 파싱된 커맨드라인 정보로 options 셋팅 ------------
	CArchiveCommandLineOptions options;
	OptionSetting( commandStrings, options );
	options.OverwriteMode = NExtract::NOverwriteMode::kWithoutPrompt; // 덮어쓰기 할때 묻지마라.
	options.YesToAll = true;
	//------------

	// 압축파일 형식 인덱스 추출.
	CIntVector formatIndices;
	if (!codecs_->FindFormatForArchiveType(options.ArcType, formatIndices))
	{
		throw kUnsupportedArcTypeMessage;
	}

	CUpdateOptions &uo = options.UpdateOptions;
	if (uo.SfxMode && uo.SfxModule.IsEmpty())
		uo.SfxModule = kDefaultSfxModule;

	COpenCallbackConsole openCallback;
	openCallback.OutStream = &outStream_;

#ifndef _NO_CRYPTO
	bool passwordIsDefined =
		options.PasswordEnabled && !options.Password.IsEmpty();
	openCallback.PasswordIsDefined = passwordIsDefined;
	openCallback.Password = options.Password;
#endif

	CUpdateCallbackConsole callback;
	callback.EnablePercents = options.EnablePercents;

#ifndef _NO_CRYPTO
	callback.PasswordIsDefined = passwordIsDefined;
	callback.AskPassword = options.PasswordEnabled && options.Password.IsEmpty();
	callback.Password = options.Password;
#endif

	callback.StdOutMode = uo.StdOutMode;
	callback.Init(&outStream_);

	CUpdateErrorInfo errorInfo;

	if (!uo.Init(codecs_, formatIndices, options.ArchiveName))
		throw kUnsupportedArcTypeMessage;

	HRESULT result = UpdateArchive(codecs_,
		options.WildcardCensor, uo,
		errorInfo, &openCallback, &callback, zdb_);

	clearDB();

	return result == S_OK;
}

void zmodifyer::clearDB()
{
	if( zdb_ )
	{
		delete zdb_;
		zdb_ = new ZDB( file_name_.GetBuffer(0) );
	}
}

//static void *ZAlloc(void *, size_t size) { return size ? malloc(size) : 0; }
//static void ZFree(void *, void *address) { if( address ) free(address); }

//static void *ZAlloc(void *, size_t size) { return umtl::memory_manager::get().alloc(size); }
//static void ZFree(void *, void *address) { umtl::memory_manager::get().free(address); }

static void *ZAlloc(void *, size_t size) { return malloc(size); }
static void ZFree(void *, void *address) { if( address ) free(address); }

static ISzAlloc alloctator = { ZAlloc, ZFree };

bool zmodifyer::compress( unsigned char const * src, size_t srcLen, unsigned char * dest, size_t & destLen, int level /* = 5 */ )
{
	if( !src || !dest || destLen < 10 )
		return false;

	CLzmaEncProps props;
	LzmaEncProps_Init(&props);
	props.level = level;
	props.algo = 0;
	props.lc = 3;
	props.lp = 0;
	props.pb = 2;

	static const unsigned int kb = 1024;

	if( srcLen > 1024 * 1024 * kb )
	{
		props.dictSize = 16 * 1024 * kb;
		props.fb = 32;
	}
	else if( srcLen > 1024 * kb )
	{
		props.dictSize = 4 * 1024 * kb;
		props.fb = 16;
	}
	else if( srcLen > kb )
	{
		props.dictSize = 64 * kb;
		props.fb = 8;
	}
	else
	{
		props.dictSize = 64 * kb;
		props.fb = 8;
	}	
	
	props.numThreads = 2;

	size_t outPropsSize = 5;

	int result = LzmaEncode(dest+10, &destLen, src, srcLen, &props, dest, &outPropsSize, 0,
		NULL, &alloctator, &alloctator);

	*(dest+5) = (unsigned char)outPropsSize;
	*(unsigned int*)(dest+6) = srcLen;
	destLen += 10;

	return (result == SZ_OK) ? true : false;
}

bool zmodifyer::uncompress( unsigned char * dest, unsigned int & destLen, const unsigned char *src, size_t srcLen)
{
	if( !src || srcLen < 10 || !dest )
		return false;

	if( destLen < *(unsigned int*)(src+6) )
		return false;

	unsigned char sizeProp = *(src+5);
	srcLen -= 10;

	ELzmaStatus status;
	int result = LzmaDecode(dest, &destLen, (src+10), &srcLen, src, sizeProp, LZMA_FINISH_ANY, &status, &alloctator);

	return (result == SZ_OK || result == SZ_ERROR_INPUT_EOF) ? true : false;
}

#undef kEverythingIsOk
#undef kUserErrorMessage
#undef kNoFormats
#undef kUnsupportedArcTypeMessage
#undef kDefaultSfxModule