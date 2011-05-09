#pragma once
/*
#include "7z/7z.h"

//////////////////////////////////////////////////////////////////////////////
//

class zfile
{
public:
	zfile( size_t index, CSzFileItem const * attr ) : index(index), attr(attr) {}

	size_t index;
	CSzFileItem const * attr;

	static void ConvertFileTimeToString(const CNtfsFileTime *ft, char *s);
	//GetAttribString(attr->AttribDefined ? attr->Attrib : 0, attr->IsDir, str);
	static void GetAttribString(UInt32 wa, Bool isDir, char *s);
	//if (attr->MTimeDefined) ConvertFileTimeToString(&attr->MTime, str);
	static char *UIntToStr(char *s, unsigned value, int numDigits);
};

//////////////////////////////////////////////////////////////////////////////
*/