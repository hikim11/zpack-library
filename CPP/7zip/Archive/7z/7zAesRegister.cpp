// 7zAesRegister.cpp

#include "7zAes.h"

#include "../Common/RegisterCodec.h"

#include "StdAfx.h"


static void *CreateCodec() { return (void *)(ICompressFilter *)(new NCrypto::NSevenZ::CDecoder()); }
#ifndef EXTRACT_ONLY
static void *CreateCodecOut() { return (void *)(ICompressFilter *)(new NCrypto::NSevenZ::CEncoder()); }
#else
#define CreateCodecOut 0
#endif

static CCodecInfo g_CodecInfo =
  { CreateCodec, CreateCodecOut, 0x06F10701, L"7zAES", 1, true };

REGISTER_CODEC(7zAES)