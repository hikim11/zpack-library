#pragma once

//#include "7z/7z.h"
//#include "7z/7zFile.h"
//#include "7z/7zCrc.h"

#include <stddef.h>
#include <string>
#include <map>
#include <unordered_map>
#include <cassert>

//#include "zfile.h"

#ifdef _DEBUG
#pragma comment(lib, "aloneD.lib")
#else
#pragma comment(lib, "alone.lib")
#endif

#include "../../7zip/ui/Console/zmodifyer.h"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <algorithm>

#include <windows.h>

//////////////////////////////////////////////////////////////////////////////
// �޸� Ǯ�� ����Ϸ��� �� �Լ��� ������ �����ϼ���.

#ifdef __cplusplus
extern "C" {
#endif

	void *SzAlloc(void *p, size_t size);
	void SzFree(void *p, void *address);

	void *SzAllocTemp(void *p, size_t size);
	void SzFreeTemp(void *p, void *address);

#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////////////////////////////
//
//typedef std::map< std::wstring, zfile > FileMap;
/*
struct FolderTree
{
	typedef boost::shared_ptr< FolderTree > FolderTreePtr;
	typedef std::map< std::wstring, FolderTreePtr > Nodes;

	std::wstring name;
	zfile * file;
	Nodes nodes;

	FolderTree() : file(0) {}
	FolderTree( std::wstring & name, zfile* file )
		: name(name), file(file)
	{}

	FolderTree * get( std::wstring & filename, zfile * f );

	FolderTree * get( std::vector< std::wstring > & path, FileMap & fileMap );

	inline void clear() { nodes.clear(); }

	inline bool empty() const { return nodes.empty(); }

	inline bool isFolder() const { return file ? false : true; }
};
//////////////////////////////////////////////////////////////////////////////
//
*/

struct WorkDirSetting
{
	WorkDirSetting( std::wstring & workDir, std::wstring & orgDir ) : workDir(workDir), orgDir( orgDir ) { 
		if( !workDir.empty() )
			SetCurrentDirectory( workDir.c_str() );
	}

	~WorkDirSetting() {
		if( !workDir.empty() && !orgDir.empty() )
			SetCurrentDirectory( orgDir.c_str() );
	}

	std::wstring & workDir;
	std::wstring & orgDir;

private:
	WorkDirSetting( WorkDirSetting const & other );
	void operator=( WorkDirSetting const & other );
};

class zpack
{
public:

	typedef std::wstring PasswordStr;

	zpack(std::wstring file_name);
	~zpack();

	/* blockIndex can have any value before first call (if outBuffer = 0) */
	/* outBuffer must be 0 before first call for each new archive. */
	/* outBufferSize can have any value before first call (if outBuffer = 0) */
	//Byte * find( std::wstring file_name, size_t & offset, size_t & outSizeProcessed,
	//	PasswordStr password = PasswordStr(), Byte * outBuffer = 0, size_t outBufferSize = 0, UInt32 blockIndex = 0xFFFFFFFF );
	// ���ϵ� �޸𸮴� �ݵ�� ������ ����� �մϴ�.

	Byte * get( UString file_name, size_t & size, PasswordStr password = PasswordStr() );

	// extract �� ���ϵ� �޸� ������
	void release( Byte * p );

	// �ش� ������ ���� ����� ��ȯ�մϴ�.
	//void folderInfo( std::vector< std::wstring > & fileList, std::wstring folderName );

	// ���� ���� (file_name.empty() �̸� ��� ���� ���� ����)
	bool extract( std::wstring file_name, std::wstring outDir = std::wstring(), bool fullPaths = true, PasswordStr password = PasswordStr() );

	// ���Ͽ� �߰�
	bool add( std::wstring file_name, int level, PasswordStr password = PasswordStr() );

	// ���Ͽ��� ����
	bool del( std::wstring file_name, PasswordStr password = PasswordStr() );

	// �ش� ������ ������ ����
	void folderInfo( std::vector< std::pair< std::wstring, ZFile* > > & fileList, wchar_t const * path, wchar_t const * password );

	// �޸� �Ҵ��� ����
	void setAlloc( void*(*a)(void*, size_t size), void(*f)(void*, void*) ) { allocImp.Alloc = a; allocImp.Free = f; }
	void setTmpAlloc( void*(*a)(void*, size_t size), void(*f)(void*, void*) ) { allocTempImp.Alloc = a; allocTempImp.Free = f; }
	
	void clear();

	void setWorkDir( wchar_t const * workDir ) { workDir_ = workDir ? workDir : L""; }

private:
	//CFileInStream archiveStream;
	//CLookToRead lookStream;
	ISzAlloc allocImp;
	ISzAlloc allocTempImp;
	//CSzArEx db;
	//SRes db_open;
	std::string file_name;
	//FileMap fileMap;
	//FolderTree folderTree_;
	zmodifyer modifyer_;
	CRITICAL_SECTION ct_;

	std::wstring workDir_;
	std::wstring orgDir_;

	//Byte * extract( size_t index, size_t & offset, size_t & outSizeProcessed,
	//	Byte * outBuffer = 0, size_t outBufferSize = 0, UInt32 blockIndex = 0xFFFFFFFF, PasswordStr password = PasswordStr() );

	void open();
	void init() ;
	void openDB();
	void close();

	//void makeFolderTree();
	//bool make_password_key( PasswordStr & password, Byte * key );
	void parse_file_names( UStringVector & names, std::wstring const & file_names );
	void parse_file_names( std::vector< std::wstring > & names, std::wstring const & file_names, std::wstring token );
};

//////////////////////////////////////////////////////////////////////////////
//

namespace zpack_util
{
	struct locker
	{
		CRITICAL_SECTION & ct_;

		locker( CRITICAL_SECTION & ct ) : ct_(ct)		{ EnterCriticalSection(&ct_); }
		~locker()										{ LeaveCriticalSection(&ct_); }

	private:
		locker( locker & other );
		void operator=(locker & other);
	};
}

//////////////////////////////////////////////////////////////////////////////