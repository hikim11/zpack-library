// dllTest.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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


	std::string testData = "���ع��� ��λ��� ������ �⵵�� �ϴ����� �����ϻ� �츮���� ���� ����ȭ ��õ�� ȭ������ ���ѻ�� �������� ���� �����ϼ� �������� �� �ҳ��� ö���� �θ��� �ٶ����� �Һ����� �츮 ����ϼ� ����ȭ ��õ�� ȭ������ ���ѻ�� �������� ���� �����ϼ�";

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

	//ZpackAdd( L"�ѱ�.7z", L"test2.cpp", 5, L"123" );
	//ZpackExtract( L"0801_battle_0102.rws.7z", L"", L"", true, L"" );

	//ZpackFolderInfo( L"�ѱ�.7z", L"a.txt", L"" );
	//ZpackExtract( L"test.rws.7z", L"", L"", true, L"" );
	ZpackExtract( L"�ѱ�.7z", L"", L"", true, L"" );

	return 0;
}

