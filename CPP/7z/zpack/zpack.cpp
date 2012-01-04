#pragma once

#include "../zpack.h"
#include "zpack.h"
#include <hash_map>
#include "utill_include.h"
#include <boost/algorithm/string.hpp>
#include <vector>
#include <algorithm>
#include <functional>
//#include <ppl.h>
//using namespace Concurrency;
#include "../parallel/for_each.hpp"

//////////////////////////////////////////////////////////////////////////////
//

#ifdef __cplusplus
extern "C" {
#endif
	void *SzAlloc(void *p, size_t size)
	{
		p = p;
		if (size == 0)
			return 0;
		return malloc(size);
	}

	void SzFree(void *p, void *address)
	{
		p = p;
		free(address);
	}

	void *SzAllocTemp(void *p, size_t size)
	{
		p = p;
		if (size == 0)
			return 0;
		return malloc(size);
	}

	void SzFreeTemp(void *p, void *address)
	{
		p = p;
		free(address);
	}
#ifdef __cplusplus
}
#endif


using namespace zpack_util;

//////////////////////////////////////////////////////////////////////////////
//

zpack::zpack(std::wstring file_name)
	//: db_open( SZ_ERROR_FAIL )
	: modifyer_(file_name.c_str())
{
	InitializeCriticalSection( &ct_ );

	this->file_name = lexical_cast< std::string >(file_name);
	/*
	memset( &db, 0, sizeof(db) );
	memset( &archiveStream, 0, sizeof(archiveStream) );
	*/

	allocImp.Alloc = SzAlloc;
	allocImp.Free = SzFree;
	allocTempImp.Alloc = SzAllocTemp;
	allocTempImp.Free = SzFreeTemp;
	
	CrcGenerateTable();

	init();

	wchar_t buf[2048];

	GetCurrentDirectory( sizeof(buf), buf );

	orgDir_ = buf;
}

//////////////////////////////////////////////////////////////////////////////
//

zpack::~zpack() {
	clear();

	DeleteCriticalSection(&ct_);
}

//////////////////////////////////////////////////////////////////////////////
//

void zpack::open()
{
	/*
	if( db_open != SZ_OK || !archiveStream.file.handle )
	{
		locker l(ct_);

		if( db_open != SZ_OK || !archiveStream.file.handle )
		{

			init();

			if (InFile_Open(&archiveStream.file, file_name.c_str()))
			{
				close();
				//throw std::exception("can not open 7z file");
			}
			else
			{
				openDB();
			}

		}
	}
	*/
}

//////////////////////////////////////////////////////////////////////////////
//

void zpack::close()
{
	/*
	if( db_open == SZ_OK )
	{
		SzArEx_Free(&db, &allocImp);
		File_Close(&archiveStream.file);
		memset(&db, 0, sizeof(db));
		memset(&archiveStream, 0, sizeof(archiveStream));
		db_open = SZ_ERROR_FAIL;
	}	

	folderTree_.clear();
	*/
}

//////////////////////////////////////////////////////////////////////////////
//

void zpack::clear()
{
	locker l(ct_);

	close();
	modifyer_.clearDB();
}

//////////////////////////////////////////////////////////////////////////////
//

void zpack::init()
{
	/*
	FileInStream_CreateVTable(&archiveStream);
	LookToRead_CreateVTable(&lookStream, False);

	lookStream.realStream = &archiveStream.s;
	LookToRead_Init(&lookStream);

	SzArEx_Init(&db);
	*/
}

//////////////////////////////////////////////////////////////////////////////
//

static void convertFilename( std::wstring & filename )
{
	int length = (int)filename.size();

	// 소문자로
	/*
	parallel::_for( 0, length, [&filename]( int i, bool & breakFlag )
		{
			if( breakFlag )
				return;

			wchar_t & c = filename[i];

			if( c == L'\\' )
			{
				c = L'/';
			}
			//else// if( iswalpha( c ) )
			//{
			//	c = (wchar_t)tolower(c);
			//}
		}
		, 4
	);
	*/

	wchar_t * c = (wchar_t*)filename.c_str();

	for( int i=0; i<length; ++i )
	{
		if( c[i] == L'\\' )
			c[i] = L'/';
	}
}

//////////////////////////////////////////////////////////////////////////////
//

void zpack::openDB() 
{
	/*
	locker l( ct_ );

	db_open = SzArEx_Open(&db, &lookStream.s, &allocImp, &allocTempImp, 0);

	if( db_open == SZ_OK )
	{
		fileMap.clear();

		static const size_t max_name = 2048;

		UInt16 temp[max_name];

		int fileNum = (int)db.db.NumFiles;

		for (int i = 0; i < fileNum; ++i)
		{
			const CSzFileItem * f = db.db.Files + i;

			SzArEx_GetFileNameUtf16(&db, i, temp);

			if( !f->IsDir )
			{
				std::wstring filename = (wchar_t *)temp;

				if( !filename.empty() )
				{
					convertFilename( filename );

					fileMap.insert( std::make_pair( filename, zfile( (size_t)i, f ) ) );
				}			
			}
		}
	}
	else
	{
		close();
	}
	*/
}

//////////////////////////////////////////////////////////////////////////////
//
/* blockIndex can have any value before first call (if outBuffer = 0) */
/* outBuffer must be 0 before first call for each new archive. */
/* outBufferSize can have any value before first call (if outBuffer = 0) */
/*
Byte* zpack::find( std::wstring filename, size_t & offset, size_t & outSizeProcessed,
	PasswordStr password, Byte * outBuffer, size_t outBufferSize, UInt32 blockIndex )
{
	open();

	convertFilename( filename );

	FileMap::iterator iter = fileMap.find( filename );

	if( iter != fileMap.end() )
	{
		return extract( iter->second.index, offset, outSizeProcessed, outBuffer, outBufferSize, blockIndex, password );
	}

	return 0;
}
*/
//////////////////////////////////////////////////////////////////////////////
//

Byte * zpack::get( UString file_name, size_t & size, PasswordStr password )
{
	locker l(ct_);

	WorkDirSetting dirSetting( workDir_, orgDir_ );

	close();

	unsigned char * p = 0;
	
	try
	{
		//convertFilename( file_name );
		//UString name(file_name.c_str());
		p = modifyer_.get( file_name, size, password.empty() ? 0 : password.c_str());
	}
	catch (...)
	{
		p = 0;
	}

	return p;
}

//////////////////////////////////////////////////////////////////////////////
//
/*
Byte * zpack::extract( size_t index, size_t & offset, size_t & outSizeProcessed,
					  Byte * outBuffer, size_t outBufferSize, UInt32 blockIndex, zpack::PasswordStr password )
{
	open();

	//Byte key[16];

	//memset(&(key[0]), 0, sizeof(key));

	//make_password_key( password, &key[0] );

	SRes res = SzArEx_Extract(&db, &lookStream.s, index
		, &blockIndex, &outBuffer, &outBufferSize,
		&offset, &outSizeProcessed,
		&allocImp, &allocTempImp, (Byte*)password.c_str());

	return (res == SZ_OK) ? outBuffer : 0;
}
*/
//////////////////////////////////////////////////////////////////////////////
//

void zpack::release( Byte * p )
{
	//if(p) IAlloc_Free(&allocImp, p);
	//if(p) delete[] p;
	locker l(ct_);

	modifyer_.release( p );
}
/*
//////////////////////////////////////////////////////////////////////////////
//
void zpack::makeFolderTree()
{
	locker l(ct_);

	if( !folderTree_.empty() )
		return;

	std::vector< std::wstring > names;

	std::for_each( fileMap.begin(), fileMap.end(), 
		[&names, this]( FileMap::value_type & file )
		{
			names.clear();

			this->parse_file_names( names, file.first, L"\\" );

			this->folderTree_.get( names, this->fileMap );
		}
	);
}
//////////////////////////////////////////////////////////////////////////////
//
void zpack::folderInfo( std::vector< std::wstring > & fileList, std::wstring folderName )
{
	open();

	std::vector< std::wstring > path;

	parse_file_names( path, folderName, L"\\/" );

	FolderTree * folder = folderTree_.get( path, fileMap );

	std::for_each( folder->nodes.begin(), folder->nodes.end(),
		[&fileList]( FolderTree::Nodes::value_type & node )
		{
			fileList.push_back( node.first );
		}
	);
}
*/

//////////////////////////////////////////////////////////////////////////////
//

void zpack::folderInfo( std::vector< std::pair< std::wstring, ZFile* > > & fileList, wchar_t const * path, wchar_t const * password )
{
	locker l(ct_);

	try
	{
		modifyer_.getInfo( fileList, path, password );
	}
	catch(...)
	{
		fileList.clear();
	}
}

//////////////////////////////////////////////////////////////////////////////
//

static void TrimLeft(std::wstring& str, const wchar_t* chars2remove = L" ")
{
	if (!str.empty())
	{
		std::wstring::size_type pos = str.find_first_not_of(chars2remove);

		if (pos != std::wstring::npos)
			str.erase(0,pos);
		else
			str.erase( str.begin() , str.end() ); // make empty
	}
}

static void TrimRight(std::wstring& str, const wchar_t* chars2remove = L" ")
{
	if (!str.empty())
	{
		std::wstring::size_type pos = str.find_last_not_of(chars2remove);

		if (pos != std::wstring::npos)
			str.erase(pos+1);
		else
			str.erase( str.begin() , str.end() ); // make empty
	}
}

//////////////////////////////////////////////////////////////////////////////
//

void zpack::parse_file_names( UStringVector & names, std::wstring const & file_names )
{
	std::vector< std::wstring > ns;

	parse_file_names( ns, file_names, L":" );

	std::for_each( ns.begin(), ns.end(),
		[&names](std::wstring & n)
		{
			if( !n.empty() )
			{
				UString name = n.c_str();
				names.Add( name );
			}
		}
	);
}

//////////////////////////////////////////////////////////////////////////////
//

void zpack::parse_file_names( std::vector< std::wstring > & names, std::wstring const & file_names, std::wstring token )
{
	boost::split( names, file_names, boost::is_any_of(token.c_str()) );

	int max_i = (int)names.size();

	/*
	parallel::_for( 0, max_i, [&names]( int i, bool & breakFlag )
		{
			if( breakFlag )
				return;

			std::wstring & name = names[i];

			TrimLeft(name);
			TrimRight(name);
			convertFilename( name );
		}
		, 4
	);
	*/

	for( int i=0; i<max_i; ++i )
	{
		std::wstring & name = names[i];

		TrimLeft(name);
		TrimRight(name);
		convertFilename(name);
	}
}

//////////////////////////////////////////////////////////////////////////////
//

bool zpack::extract( std::wstring file_names, std::wstring outDir, bool fullPaths, zpack::PasswordStr password )
{
	locker l(ct_);

	WorkDirSetting dirSetting( workDir_, orgDir_ );

	bool result = false;

	close();
	
	UStringVector names;

	parse_file_names( names, file_names );

	try
	{
		result = modifyer_.extract( names, password.empty() ? 0 : password.c_str(), outDir.empty() ? 0 : outDir.c_str(), fullPaths );
	}
	catch (...)
	{
		result = false;
	}	

	return result;
}

//////////////////////////////////////////////////////////////////////////////
//

bool zpack::add(std::wstring file_name, int level, PasswordStr password /* = PasswordStr */)
{
	locker l(ct_);

	WorkDirSetting dirSetting( workDir_, orgDir_ );

	bool result = true;

	close();

	UStringVector names;

	parse_file_names( names, file_name );

	try
	{
		modifyer_.add( names, level, password.empty() ? 0 : password.c_str() );
	}
	catch (...)
	{
		result = false;
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////
//

bool zpack::del(std::wstring file_name, PasswordStr password /* = PasswordStr */)
{
	locker l(ct_);

	WorkDirSetting dirSetting( workDir_, orgDir_ );

	bool result = true;

	close();

	UStringVector names;

	parse_file_names( names, file_name );

	try
	{
		modifyer_.del( names, password.empty() ? 0 : password.c_str() );
	}
	catch (...)
	{
		result = false;
	}

	return result;
}

//////////////////////////////////////////////////////////////////////////////
//
/*
bool zpack::make_password_key( PasswordStr & password, Byte * key )
{
	if( !key || password.empty() )
		return false;

	PasswordStr ps1 = password;

	size_t key1 = stdext::hash_value(ps1);

	size_t * k = (size_t*)(&key[0]);

	if( ps1.length() < 2 )
	{
		for(int i=0; i<4; ++i)
			k[i] = key1;

		return true;
	}

	std::sort( ps1.begin(), ps1.end() );

	PasswordStr ps2 = password;

	std::reverse( ps2.begin(), ps2.end() );

	k[0] = stdext::hash_value( ps1 );
	k[1] = stdext::hash_value( ps2 );
	k[2] = k[0] ^ k[1];
	k[3] = key1;

	return true;
}
//////////////////////////////////////////////////////////////////////////////
//
FolderTree * FolderTree::get( std::wstring & filename, zfile * f ) {

	FolderTree * folderTree = 0;

	Nodes::iterator iter = nodes.find( filename );

	if( iter != nodes.end() )
	{
		folderTree = iter->second.get();
	}
	else
	{
		folderTree = new FolderTree( filename, f );
		nodes[filename].reset( folderTree );
	}

	return folderTree;
}

//////////////////////////////////////////////////////////////////////////////
//

FolderTree * FolderTree::get( std::vector< std::wstring > & path, FileMap & fileMap )
{
	FolderTree * tree = this;

	std::wstring curName = L"";

	for( size_t i=0; i<path.size(); ++i )
	{
		if( i != 0 )
			curName += L"\\";

		if( path[i].empty() )
			continue;

		curName += path[i];

		FileMap::iterator iter = fileMap.find( curName );

		zfile * f = ( iter != fileMap.end() ) ? &iter->second : 0;

		tree = tree->get( curName, f );
	}

	return tree;
}
*/

//////////////////////////////////////////////////////////////////////////////