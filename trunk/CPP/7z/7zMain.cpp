/* 7zMain.c - Test application for 7z Decoder
2010-03-12 : Igor Pavlov : Public domain */

#include "zpack.h"
#include "zpack/zpack.h"
#include <iostream>

#include <hash_map>
#include <string>

#include <algorithm>
#include <ppl.h>
using namespace Concurrency;

#include <Windows.h>

#include "utill_include.h"

#include <boost/timer.hpp>

void print_help()
{
	std::cout << "압축파일.zp (-p비번) 명령 파일1:파일2:파일3..." << std::endl;
	std::cout << "명령 : a 추가, d 삭제, e 압축해제, o 출력" << std::endl;
}

#ifdef _NOTDLL
int MY_CDECL main(int numargs, char *args[])
{
	unsigned char buf[1024] = {0,};
	size_t destSize = sizeof(buf);

	unsigned char buf2[1024] = {0,};
	size_t destSize2 = sizeof(buf2);

	boost::timer t;

	std::string testData = "동해물과 백두산이 마르고 닳도록 하느님이 보우하사 우리나라 만세 무궁화 삼천리 화려강산 대한사람 대한으로 길이 보전하세 남산위에 저 소나무 철갑을 두른듯 바람서리 불변함은 우리 기상일세 무궁화 삼천리 화려강산 대한사람 대한으로 길이 보전하세";

	for(int i = 0; i < 10000; ++i )
	{
		//std::cout << testData.length() << std::endl;

		zmodifyer::compress( (unsigned char const *)testData.c_str(), testData.size(), buf, destSize );

		//std::cout << destSize << std::endl;	

		zmodifyer::uncompress( buf2, destSize2, buf, destSize );

		//std::cout << buf2 << std::endl;
	}

	std::cout << t.elapsed() << std::endl;

	/*
	if( numargs > 2 )
	{
		zpack pack( lexical_cast< std::wstring >(args[1]) );

		std::wstring password;

		std::wstring option = lexical_cast< std::wstring >( args[2] );

		int idx = 2;

		if( (option.length() > 2) && (option.find( L"-p" ) != std::wstring::npos) )
		{
			password = option.substr( 2, std::wstring::npos );
			++idx;
		}

		std::string command = args[idx++];

		if( command == "e" )
		{
			pack.extract( (numargs > 3) ? lexical_cast<std::wstring>( args[idx] ) : L"", L"", true, password );
		}
		else if( command == "l" )
		{
			std::vector< std::pair<std::wstring, ZFile*> > fileList;

			pack.folderInfo( fileList, (numargs > 3) ? lexical_cast<std::wstring>( args[idx] ).c_str() : L"", password.empty() ? 0 : password.c_str() );

			std::for_each( fileList.begin(), fileList.end(),
				[](std::pair<std::wstring, ZFile*> & file)
				{
					std::wcout << file.first << std::endl;
				}
			);

			std::cout << fileList.size() << std::endl;
		}
		else if( numargs > 3 )
		{
			std::wstring files = lexical_cast< std::wstring >( (char const *)(args[idx]) );

			if( command == "a" )
			{
				int start = GetTickCount();

				pack.add( files, 5, password );

				std::cout << (GetTickCount() - start) * 0.001f << std::endl;
			}
			else if( command == "d" )
			{
				pack.del( files, password );
			}
			else if( command == "g" || command == "o" )
			{
				size_t size;

				unsigned char * p = (unsigned char*)pack.get( files, size, password );

				if( p )
				{
					char * s = new char[size+1];

					memcpy_s( s, size, p, size );

					s[size] = 0;

					std:: cout << s << std::endl;

					delete[] s;

					pack.release( (unsigned char *)p );
				}
			}
// 			else if( command == "o" )
// 			{
// 				size_t offset;
// 				size_t size;
// 
// 				char * p = (char *)pack.find( files, offset, size, password );
// 
// 				if( p )
// 				{
// 					char * s = new char[size+1];
// 
// 					memcpy_s( s, size, p + offset, size );
// 
// 					s[size] = 0;
// 
// 					std::cout << s << std::endl;
// 
// 					delete[] s;
// 
// 					pack.release((unsigned char *)p);
// 				}
// 			}
			else
			{
				print_help();
			}
		}
		else
		{
			print_help();
		}
	}
	else
	{
		print_help();
	}
	*/
		
	return 0;
}
#endif

namespace zpack_util
{
	class zpack_manager
	{
		typedef std::map< std::wstring, zpack* > zpacks;
		typedef std::map< unsigned char *, unsigned char * > streams;

		zpacks zpacks_;
		streams streams_;

	public:
		CRITICAL_SECTION ct_;

		zpack_manager() { InitializeCriticalSection(&ct_); }
		~zpack_manager() {

			std::for_each( streams_.begin(), streams_.end(),
				[]( streams::value_type& stream ) {
					if( stream.second )
						delete stream.second;
				}
			);

			std::for_each( zpacks_.begin(), zpacks_.end(),
				[]( zpacks::value_type& pack ) {
					if( pack.second )
						delete pack.second;
				}
			);

			DeleteCriticalSection(&ct_);
		}

		zpack * get( std::wstring pack_name ) 
		{
			locker l(ct_);

			zpack * pack = 0;

			// 소문자로
			/*
			parallel::_for( 0, (int)pack_name.length(),
				[&pack_name](int i, bool & breakFlag)
				{
					if( breakFlag )
						return;

					wchar_t & c = pack_name[i];

					c = (wchar_t)tolower(c);
				}
				, pack_name.length() < 10 ? 2 : 4
			);
			*/

			std::transform( pack_name.begin(), pack_name.end(), pack_name.begin(), towlower );

			zpacks::iterator iter = zpacks_.find( pack_name );

			if( iter != zpacks_.end() )
			{
				pack = iter->second;
			}
			else
			{
				pack = new zpack(pack_name);

				zpacks_.insert( std::make_pair( pack_name, pack ) );
			}

			return pack;
		}
		/*
		void registStream( unsigned char * p, size_t offset )
		{
			locker l(ct_);

			if(!p)
				return;

			streams_.insert( std::make_pair( p + offset, p ) );
		}

		unsigned char * unRegistStream( unsigned char * p )
		{
			locker l(ct_);

			unsigned char * rp = 0;

			if( p )
			{
				streams::iterator iter = streams_.find( p );

				if( iter != streams_.end() )
				{
					rp = iter->second;
					streams_.erase(iter);
				}
			}			

			return rp;
		}
		*/
	};

	static zpack_manager zpackmanager;
}

using namespace zpack_util;

#ifndef _NOTDLL
extern "C" LIBSPEC unsigned char * ZpackFind( wchar_t const * pack_file, wchar_t const * file_name, unsigned int & outSizeProcessed, wchar_t const * password )
{
	unsigned char * result = 0;

	if( pack_file && file_name )
	{
		zpack_util::locker l( zpackmanager.ct_ ); // CIOStream 이 static 이라서..

		if(!password)
			password = L"";

		zpack * pack = zpackmanager.get( pack_file );

		if( pack )
		{
			size_t offset = 0;

			//result = (unsigned char*)(pack->find( file_name, offset, outSizeProcessed, password ));
			result = (unsigned char*)(pack->get( file_name, outSizeProcessed, password ));

			if( result )
			{
				//zpackmanager.registStream( result, offset );
				result = result + offset;
			}
		}
	}	

	return result;
}

extern "C" LIBSPEC bool ZpackDelete( wchar_t const * pack_file, wchar_t const * file_name, wchar_t const * password )
{
	bool result = false;

	if( pack_file && file_name )
	{
		if( !password )
			password = L"";

		zpack * pack = zpackmanager.get( pack_file );

		if( pack )
		{
			result = pack->del( file_name, password );
		}
	}

	return result;
}

extern "C" LIBSPEC bool ZpackAdd( wchar_t const * pack_file, wchar_t const * file_name, int level, wchar_t const * password )
{
	bool result = false;

	if( pack_file && file_name )
	{
		if( !password )
			password = L"";

		zpack * pack = zpackmanager.get( pack_file );

		if(pack)
		{
			result = pack->add( file_name, level, password );
		}
	}	

	return result;
}

extern "C" LIBSPEC bool ZpackExtract( wchar_t const * pack_file, wchar_t const * file_name, wchar_t const * outDir, bool fullPath, wchar_t const * password )
{
	bool result = false;

	if( pack_file )
	{
		if( !password )
			password = L"";

		zpack * pack = zpackmanager.get( pack_file );

		if(pack)
		{
			result = pack->extract( file_name ? file_name : L"", outDir ? outDir : L"", fullPath, password );
		}
	}	

	return result;
}

extern "C" LIBSPEC void ZpackRelease( wchar_t const * pack_file, wchar_t const * buffer )
{
	if( pack_file && buffer )
	{
		zpack * pack = zpackmanager.get( pack_file );

		if(pack)
		{
			//unsigned char * rp = zpackmanager.unRegistStream( (unsigned char *)buffer );
			unsigned char * rp = (unsigned char *)buffer;
			pack->release( rp );
		}
	}
}

/*
extern "C" LIBSPEC void ZpackSetAllocor( wchar_t const * pack_file, void*(*allocFunc)(void*,size_t), void(*freeFunc)(void*,void*) )
{
	if( pack_file && allocFunc && freeFunc )
	{
		zpack * pack = zpackmanager.get( pack_file );

		if( pack )
		{
			pack->setAlloc( allocFunc, freeFunc );
		}
	}
}

extern "C" LIBSPEC void ZpackSetTmpAllocor( wchar_t const * pack_file, void*(*allocFunc)(void*,size_t), void(*freeFunc)(void*,void*) )
{
	if( pack_file && allocFunc && freeFunc )
	{
		zpack * pack = zpackmanager.get( pack_file );

		if( pack )
		{
			pack->setTmpAlloc( allocFunc, freeFunc );
		}
	}
}
*/

extern "C" LIBSPEC wchar_t const * ZpackFolderInfo( wchar_t const * pack_file, wchar_t const * path, wchar_t const * password )
{
	zpack_util::locker l( zpackmanager.ct_ );

	static std::wstring result = L"";

	result = L"";

	if( pack_file )
	{
		typedef std::pair< std::wstring, ZFile* > FileInfo;

		std::vector< FileInfo > infos;
		
		zpack * pack = zpackmanager.get( pack_file );

		if( pack )
		{
			pack->folderInfo( infos, path, password );
		}

		for(size_t i=0; i<infos.size(); ++i)
		{
			if( i != 0 )
				result += L"\n";

			FileInfo & fileinfo = infos[i];

			ZFile * zfile = fileinfo.second;

			result += fileinfo.first;

			if( zfile && zfile->fileInfo_ )
			{
				std::wstringstream stream;
				stream << L"\tisDir-" << (zfile->fileInfo_->IsDir ? 1 : 0) << L"\tsize-" << zfile->fileInfo_->Size << L"\tcrc-" << zfile->fileInfo_->Crc << L"\tattrId-" << zfile->fileInfo_->Attrib;

				result += stream.str();
			}
		}
	}

	return result.c_str();
}


extern "C" LIBSPEC void ZpackWorkDir( wchar_t const * pack_file, wchar_t const * workDir )
{
	if( pack_file )
	{
		zpack * pack = zpackmanager.get( pack_file );

		if( pack )
		{
			pack->setWorkDir( workDir );
		}
	}
}

extern "C" LIBSPEC void ZpackClearCache( wchar_t const * pack_file )
{
	if( pack_file )
	{
		zpack * pack = zpackmanager.get( pack_file );

		if( pack )
		{
			pack->clear();
		}
	}
}


extern "C" LIBSPEC bool CompressLZMA( unsigned char const * src, unsigned int srcLen, unsigned char * dest, unsigned int &destLen, int level )
{
	return zmodifyer::compress( src, srcLen, dest, destLen, level );
}

extern "C" LIBSPEC bool UnCompressLZMA( unsigned char const * src, unsigned int srcLen, unsigned char * dest, unsigned int &destLen )
{
	return zmodifyer::uncompress( dest, destLen, src, srcLen );
}

BOOL WINAPI DllMain(HINSTANCE hInst, DWORD fdwReason, LPVOID lpRes)
{

	//변수 선언 및 필요 기능 구현
	switch(fdwReason)
	{
	case DLL_PROCESS_ATTACH:

		//DLL이 프로세스(즉, 메인 스레드)의 주소 공간에 맵핑될 경우 호출된다.
		//주로 메모리를 할당하거나 시스템 전역 핸들을 초기화하는 용도로 사용된다.
		//암시적 호출일 경우 프로세스가 시작될 때,
		//명시적 호출 일 경우 LoadLibrary()가 리턴되기 전에 이 값과 같이 DllMain()이 호출된다.

		break; 

	case DLL_PROCESS_DETACH:

		//DLL이 프로세스의 주소 공간에서 분리될 때 호출된다.
		//주로 DLL_PROCESS_ATTACH에서 할당된 메모리 해제나
		//시스템 전역 핸들을 제거하는 용도로 사용된다.
		//암시적 호출일 경우 프로세스가 종료될 때, 
		//명시적 호출일 경우 FreeLibrary() 함수가 호출된 경우
		//이 값과 DllMain()이 호출된다.

		break; 

	case DLL_THREAD_ATTACH:

		//DLL을 사용하는 클라이언트 프로세스에서 스레드를 생성할 때마다
		//이 값과 함께 DllMain() 함수가 호출된다.
		//DLL에서는 이 메시지를 받았을 경우 스레드 별 초기화를 수행해야한다.
		//그리고 이미 존재하는 스레드에 대해서는 전달되지 않으며
		//스레드가 새로 생성될 때만 전달된다. 
		//최상위 스레드(즉, 메인 스레드)는 DLL_PROCESS_ATTACH에서 처리되므로 이 메시지가 발생하지 않는다.

		break; 

	case DLL_THREAD_DETACH:

		//DLL을 사용하는 클라이언트 프로세스에서 스레드가 종료될 때마다
		//이 값과 함께 DllMain() 함수가 호출된다.
		//DLL에서는 이 값을 받았을 때 스레드 별로 종료 처리를 수행한다. 
		//DLL 로드 전 존재했던 스레드는 DLL_THREAD_ATTACH 메시지 처리가 
		//되지 않았을 것이기 때문에 해제해야 할 경우
		//초기화가 된 것인지 확인 한 후 해제 할 것만 처리한다.

		break; 

	}

	return TRUE;
}

#endif