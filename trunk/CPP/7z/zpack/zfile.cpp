/*
#include "zfile.h"


//////////////////////////////////////////////////////////////////////////////
//


char * zfile::UIntToStr(char *s, unsigned value, int numDigits)
{
	char temp[16];
	int pos = 0;
	do
	temp[pos++] = (char)('0' + (value % 10));
	while (value /= 10);
	for (numDigits -= pos; numDigits > 0; numDigits--)
		*s++ = '0';
	do
	*s++ = temp[--pos];
	while (pos);
	*s = '\0';
	return s;
}

//////////////////////////////////////////////////////////////////////////////
//

void zfile::ConvertFileTimeToString(const CNtfsFileTime *ft, char *s)
{
	static const int PERIOD_4 = (4 * 365 + 1);
	static const int PERIOD_100 = (PERIOD_4 * 25 - 1);
	static const int PERIOD_400 = (PERIOD_100 * 4 + 1);

	unsigned year, mon, day, hour, min, sec;
	UInt64 v64 = (ft->Low | ((UInt64)ft->High << 32)) / 10000000;
	Byte ms[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	unsigned t;
	UInt32 v;
	sec = (unsigned)(v64 % 60); v64 /= 60;
	min = (unsigned)(v64 % 60); v64 /= 60;
	hour = (unsigned)(v64 % 24); v64 /= 24;

	v = (UInt32)v64;

	year = (unsigned)(1601 + v / PERIOD_400 * 400);
	v %= PERIOD_400;

	t = v / PERIOD_100; if (t ==  4) t =  3; year += t * 100; v -= t * PERIOD_100;
	t = v / PERIOD_4;   if (t == 25) t = 24; year += t * 4;   v -= t * PERIOD_4;
	t = v / 365;        if (t ==  4) t =  3; year += t;       v -= t * 365;

	if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
		ms[1] = 29;
	for (mon = 1; mon <= 12; mon++)
	{
		unsigned s = ms[mon - 1];
		if (v < s)
			break;
		v -= s;
	}
	day = (unsigned)v + 1;
	s = UIntToStr(s, year, 4); *s++ = '-';
	s = UIntToStr(s, mon, 2);  *s++ = '-';
	s = UIntToStr(s, day, 2);  *s++ = ' ';
	s = UIntToStr(s, hour, 2); *s++ = ':';
	s = UIntToStr(s, min, 2);  *s++ = ':';
	s = UIntToStr(s, sec, 2);
}

void zfile::GetAttribString(UInt32 wa, Bool isDir, char *s)
{
	s[0] = (char)(((wa & FILE_ATTRIBUTE_DIRECTORY) != 0 || isDir) ? 'D' : '.');
	s[1] = (char)(((wa & FILE_ATTRIBUTE_READONLY) != 0) ? 'R': '.');
	s[2] = (char)(((wa & FILE_ATTRIBUTE_HIDDEN) != 0) ? 'H': '.');
	s[3] = (char)(((wa & FILE_ATTRIBUTE_SYSTEM) != 0) ? 'S': '.');
	s[4] = (char)(((wa & FILE_ATTRIBUTE_ARCHIVE) != 0) ? 'A': '.');
	s[5] = '\0';
}

//////////////////////////////////////////////////////////////////////////////
*/