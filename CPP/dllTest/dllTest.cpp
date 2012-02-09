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
	/*
	int start = GetTickCount();

	unsigned char buf[1024] = {0,};
	size_t destSize = sizeof(buf);

	unsigned char buf2[1024] = {0,};
	size_t destSize2 = sizeof(buf2);


	std::string testData = "동해물과 백두산이 마르고 닳도록 하느님이 보우하사 우리나라 만세 무궁화 삼천리 화려강산 대한사람 대한으로 길이 보전하세 남산위에 저 소나무 철갑을 두른듯 바람서리 불변함은 우리 기상일세 무궁화 삼천리 화려강산 대한사람 대한으로 길이 보전하세";

	for(int i = 0; i < 10000; ++i )
	{
		//std::cout << testData.length() << std::endl;

		CompressLZMA( (unsigned char const *)testData.c_str(), testData.size(), buf, destSize );

		//std::cout << destSize << std::endl;	

		UnCompressLZMA( buf, destSize, buf2, destSize2 );

		//std::cout << buf2 << std::endl;
	}

	//std::cout << (GetTickCount() - start) * 0.001f << std::endl;

	*/

	

	//std::string str;

	/*
	for( int i=0; i<1; ++i )
	{
		ZpackExtract( L"console/222.7z", L"doc/src", L"console", true, L"123" );
		std::wcout << ZpackFolderInfo( L"console/222.7z", 0, L"123" ) << std::endl;

		std::fstream fs("boost\\more\\BoostSponsorshipAgreemenT.pdf");

		str.clear();

		char buf[1024] = { 0, };

		while( fs.good() )
		{
			fs.get( buf, sizeof(buf) );
			str += buf;
		}
	}
	*/

	/*
	std::wstring s = ZpackFolderInfo( L"a.zp", L"", 0 );
	std::wcout << s << std::endl;
	*/

	//ZpackAdd( L"한글.7z", L"test2.cpp", 5, L"123" );
	//ZpackExtract( L"0801_battle_0102.rws.7z", L"", L"", true, L"" );

	//ZpackFolderInfo( L"한글.7z", L"a.txt", L"" );
	//ZpackExtract( L"test.rws.7z", L"", L"", true, L"" );
	ZpackExtract( L"한글.7z", L"", L"", true, L"" );

	return 0;
}

