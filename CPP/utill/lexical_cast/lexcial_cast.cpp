#include "lexical_cast.h"

//namespace umtl
//{
	//////////////////////////////////////////////////////////////////////////
	//

	std::string to_utf8( std::string & text )
	{
		typedef boost::program_options::detail::utf8_codecvt_facet utf8_codecvt;

		std::wstring wtext = lexical_cast< std::wstring >(text);

		std::locale loc(std::locale(""), new utf8_codecvt() );

		return narrower( loc )(wtext);
	}

	//////////////////////////////////////////////////////////////////////////
//}