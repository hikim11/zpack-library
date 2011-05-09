// OpenCallbackConsole.h

#ifndef __OPENCALLBACKCONSOLE_H
#define __OPENCALLBACKCONSOLE_H

#include "Common/StdOutStream.h"
#include "../Common/ArchiveOpenCallback.h"
#include "../../ICoder.h"

class COpenCallbackConsole: public IOpenCallbackUI
{
public:
	INTERFACE_IOpenCallbackUI(;)
	CStdOutStream *OutStream;

#ifndef _NO_CRYPTO
	bool PasswordIsDefined;
	bool PasswordWasAsked;
	UString Password;
	COpenCallbackConsole(): PasswordIsDefined(false), PasswordWasAsked(false) {}
	~COpenCallbackConsole() {}
#endif
};

class CCryptoGetTextPassword : public ICryptoGetTextPassword, public CMyUnknownImp
{	
public:
	static UString Password;

	MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

	STDMETHOD(CryptoGetTextPassword)(BSTR *password);
};

class CCryptoSetPassword : public ICryptoSetPassword, public CMyUnknownImp
{
public:

	MY_UNKNOWN_IMP1(ICryptoSetPassword)

	STDMETHOD(CryptoSetPassword)(const Byte *data, UInt32 size);
};

#endif
