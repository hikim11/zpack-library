#pragma once

#include "Bcj2Coder.h"

#include "../Common/RegisterCodec.h"

class Bcj2CodecRegister
{
public:

	static void *CreateBcj2Codec() { return (void *)(ICompressCoder2 *)(new NCompress::NBcj2::CDecoder()); }
#ifndef EXTRACT_ONLY
	static void *CreateBcj2CodecOut() { return (void *)(ICompressCoder2 *)(new NCompress::NBcj2::CEncoder());  }
#else
	static void *CreateBcj2CodecOut() { return 0; }
#endif

	static CCodecInfo _codecinfo;

	struct Register { 
		Register() { RegisterCodec(&Bcj2CodecRegister::_codecinfo); }
	};
	
	static Register _register;
};
