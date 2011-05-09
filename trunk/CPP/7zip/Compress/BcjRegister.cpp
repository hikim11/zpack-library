// BcjRegister.cpp

#include "StdAfx.h"

#include "BcjRegister.h"

CCodecInfo BcjCodecRegister::_codecinfo = 
	{ BcjCodecRegister::CreateBcjCodec, BcjCodecRegister::CreateBcjCodecOut, 0x03030103, L"BCJ", 1, true };

struct Register { 
	Register() { RegisterCodec(&BcjCodecRegister::_codecinfo); }
};

BcjCodecRegister::Register BcjCodecRegister::_register;