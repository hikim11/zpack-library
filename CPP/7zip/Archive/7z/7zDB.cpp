#include "7zDB.h"
#include "7zItem.h"
#include <boost/algorithm/string.hpp>
#include "parallel/for_each.hpp"
#include "utill_include.h"

//---------------------------------------------------------
//

ZFile::ZFile(UString & name) : name_(name), index_(-1), fileInfo_(0) {}

//---------------------------------------------------------
//

ZFile::~ZFile() {
	std::for_each( elements_.begin(), elements_.end(),
		[]( Elements::value_type & elem )
		{
			delete elem.second;
		}
	);
}

//---------------------------------------------------------
//

ZFile * ZFile::insert( UString const & filename, int index, NArchive::N7z::CFileItem * fileInfo )
{
	if( filename.Length() == 0 )
		return this;

	FolderPath path;

	std::wstring tmp = filename;

	boost::split( path, tmp, boost::is_any_of(L"/\\") );

	return insert( path, index, fileInfo );
}

//---------------------------------------------------------
//

ZFile * ZFile::insert( ZFile::FolderPath & path, int index, NArchive::N7z::CFileItem * fileInfo )
{
	ZFile * file = 0;

	if( !path.empty() )
	{
		UString filename = path.front().c_str();

		Elements::iterator iter = elements_.find( filename );

		if( iter != elements_.end() )
		{
			file = iter->second;

			path.pop_front();

			file = file->insert( path, index, fileInfo );
		}
		else
		{
			ZFile * file = new ZFile( filename );

			elements_.insert( std::make_pair( filename, file ) );

			path.pop_front();

			file = file->insert( path, index, fileInfo );
		}
	}
	else
	{
		index_ = index;
		file = this;
		fileInfo_ = fileInfo;
	}

	return file;
}

//---------------------------------------------------------
//

ZFile * ZFile::find( UString const & filename )
{
	if( filename.Length() == 0 )
		return 0;

	FolderPath path;

	std::wstring tmp = filename;

	if( tmp.back() == L'\\' || tmp.back() == L'/' )
		tmp.pop_back();

	boost::split( path, tmp, boost::is_any_of(L"/\\") );

	return find( path );
}

//---------------------------------------------------------
//

ZFile * ZFile::find( std::wstring const & filename )
{
	if( filename.empty() )
		return 0;

	FolderPath path;

	boost::split( path, filename, boost::is_any_of(L"/\\") );

	if( path.back().empty() )
		path.pop_back();

	return find( path );
}

//---------------------------------------------------------
//

ZFile * ZFile::find( umtl::static_pool::wstring const & filename )
{
	if( filename.empty() )
		return 0;

	FolderPath path;

	boost::split( path, filename, boost::is_any_of(L"/\\") );

	if( path.back().empty() )
		path.pop_back();

	return find( path );
}

//---------------------------------------------------------
//

ZFile * ZFile::find( ZFile::FolderPath & path )
{
	ZFile * file = 0;

	if( !path.empty() )
	{
		UString filename = path.front().c_str();

		Elements::iterator iter = elements_.find( filename );

		if( iter != elements_.end() )
		{
			file = iter->second;

			path.pop_front();

			return file->find( path );
		}
		else
		{
			return 0;
		}
	}
	else
	{
		file = this;
	}

	return file;
}

//---------------------------------------------------------
//

void ZFile::getList( std::vector< std::wstring > & fileList, std::wstring path, bool isRoot )
{
	if( !path.empty() && path.back() != L'/' && path.back() != L'\\' )
	{
		path += L"/";
	}

	if( !isRoot )
		path += name_;

	if( elements_.empty() )
	{
		fileList.push_back( path );
	}
	else
	{
		std::for_each( elements_.begin(), elements_.end(),
			[ &fileList, &path ]( Elements::value_type & elem )
			{
				elem.second->getList( fileList, path, false );
			}
		);
	}
}

//--------------------------------------------------
//

void ZFile::getList( std::vector< std::pair< std::wstring, ZFile * > > & fileList, std::wstring path, bool isRoot )
{
	if( !path.empty() && path.back() != L'/' && path.back() != L'\\' )
	{
		path += L"/";
	}

	if( !isRoot )
		path += name_;

	if( elements_.empty() )
	{
		fileList.push_back( std::make_pair( path, this ) );
	}
	else
	{
		std::for_each( elements_.begin(), elements_.end(),
			[ &fileList, &path ]( Elements::value_type & elem )
		{
			elem.second->getList( fileList, path, false );
		}
		);
	}
}

//---------------------------------------------------------
//

ZDB::ZDB( UString archiveName )
	: archiveName_(archiveName)
	, folder_(archiveName)
{}

//---------------------------------------------------------
//

void ZDB::insert( UString & filename, int index, NArchive::N7z::CFileItem * fileInfo )
{
	if( filename.Length() == 0 )
		return;

	folder_.insert( filename, index, fileInfo );
}

//---------------------------------------------------------