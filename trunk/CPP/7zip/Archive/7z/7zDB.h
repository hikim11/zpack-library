#pragma once

#include <string>
#include <map>
#include <list>
#include <vector>
#include <algorithm>
#include "7zIn.h"
#include "parallel/for_each.hpp"

//---------------------------------------------------------
//

class ZFile
{
public:
	typedef std::map< UString, ZFile * > Elements;
	typedef std::list< std::wstring > FolderPath;

	Elements elements_;
	UString name_;
	int index_;

	ZFile(UString & name);
	~ZFile();

	ZFile * insert( UString const & filename, int index );
	ZFile * find( UString const & filename );
	ZFile * find( std::wstring const & filename );

	void	getList( std::vector< std::wstring > & fileList, std::wstring path, bool isRoot = true );

	template< typename Func >
	ZFile & operator()( Func & func )
	{
		func(*this);

		std::for_each( elements_.begin(), elements_.end(),
			[&func]( Elements::value_type & elem )
			{
				(*elem.second)( func );
			}
		);

		return *this;
	}

private:
	ZFile * insert( FolderPath & path, int index );
	ZFile * find( FolderPath & path );
};

//---------------------------------------------------------
//

class ZDB
{
public:
	typedef NArchive::N7z::CArchiveDatabaseEx Database;

	Database db_;
	ZFile folder_;
	UString const archiveName_;

	ZDB( UString archiveName );

	void insert( UString & filename, int index );
};

//---------------------------------------------------------
