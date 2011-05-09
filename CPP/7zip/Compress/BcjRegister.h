#pragma once

#include "BcjCoder.h"

#include "../Common/RegisterCodec.h"

class BcjCodecRegister
{
public:

	static void *CreateBcjCodec() { return (void *)(ICompressFilter *)(new CBCJ_x86_Decoder()); }
#ifndef EXTRACT_ONLY
	static void *CreateBcjCodecOut() { return (void *)(ICompressFilter *)(new CBCJ_x86_Encoder());  }
#else
	static void *CreateBcjCodecOut() { return 0; }
#endif

	static CCodecInfo _codecinfo;

	struct Register { 
		Register() { RegisterCodec(&BcjCodecRegister::_codecinfo); }
	};

	static Register _register;
};
