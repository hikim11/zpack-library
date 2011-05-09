// Bcj2Register.cpp

#include "StdAfx.h"

#include "Bcj2Register.h"

CCodecInfo Bcj2CodecRegister::_codecinfo = 
	{ Bcj2CodecRegister::CreateBcj2Codec, Bcj2CodecRegister::CreateBcj2CodecOut, 0x0303011B, L"BCJ2", 4, false };

Bcj2CodecRegister::Register Bcj2CodecRegister::_register;