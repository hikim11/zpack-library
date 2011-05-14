#pragma once

#ifdef _COMPLING_DLL_
#define LIBSPEC __declspec(dllexport)
#else
#define LIBSPEC __declspec(dllimport)
#endif

extern "C" LIBSPEC unsigned char * ZpackFind( wchar_t const * pack_file, wchar_t const * file_name, unsigned int & outSizeProcessed, wchar_t const * password );
extern "C" LIBSPEC bool ZpackDelete( wchar_t const * pack_file, wchar_t const * file_name, wchar_t const * password );
extern "C" LIBSPEC bool ZpackAdd( wchar_t const * pack_file, wchar_t const * file_name, int level, wchar_t const * password );
extern "C" LIBSPEC bool ZpackExtract( wchar_t const * pack_file, wchar_t const * file_name, wchar_t const * outDir, bool fullPath, wchar_t const * password );
extern "C" LIBSPEC void ZpackRelease( wchar_t const * pack_file, wchar_t const * buffer );
extern "C" LIBSPEC wchar_t const * ZpackFolderInfo( wchar_t const * pack_file, wchar_t const * path, wchar_t const * password );
extern "C" LIBSPEC void ZpackWorkDir( wchar_t const * pack_file, wchar_t const * workDir );
extern "C" LIBSPEC void ZpackClearCache( wchar_t const * pack_file );

extern "C" LIBSPEC bool CompressLZMA( unsigned char const * src, unsigned int srcLen, unsigned char * dest, unsigned int &destLen, int level = 5 );
extern "C" LIBSPEC bool UnCompressLZMA( unsigned char const * src, unsigned int srcLen, unsigned char * dest, unsigned int &destLen );

//extern "C" LIBSPEC void ZpackSetAllocor( wchar_t const * pack_file, void*(*allocFunc)(void*,size_t), void(*freeFunc)(void*,void*) );
//extern "C" LIBSPEC void ZpackSetTmpAllocor( wchar_t const * pack_file, void*(*allocFunc)(void*,size_t), void(*freeFunc)(void*,void*) );