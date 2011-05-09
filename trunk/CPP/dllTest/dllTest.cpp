// dllTest.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "zpack.h"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <boost/algorithm/string.hpp>

int _tmain(int argc, _TCHAR* argv[])
{
	size_t size;

	int start = GetTickCount();

	//std::string str;

	for( int i=0; i<1; ++i )
	{
		//wchar_t const * info = ZpackFolderInfo( L"console/222.7z", L"", L"" );
		ZpackExtract( L"console/222.7z", L"doc/src", L"console", true, L"123" );

		/*
		std::fstream fs("boost\\more\\BoostSponsorshipAgreemenT.pdf");

		str.clear();

		char buf[1024] = { 0, };

		while( fs.good() )
		{
			fs.get( buf, sizeof(buf) );
			str += buf;
		}
		*/
	}

	std::cout << (GetTickCount() - start) * 0.001f << std::endl;

	return 0;
}

