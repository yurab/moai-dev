// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef USHTTPTASKINFO_CURL_H
#define USHTTPTASKINFO_CURL_H
#ifndef MOAI_OS_NACL
#define USE_CURL 1
#endif

#ifdef USE_CURL

#define  CURL_STATICLIB
#define  CURL_DISABLE_LDAP

extern "C" {
	#include <curl/curl.h>
}

//================================================================//
// USHttpTaskInfo
//================================================================//
class USHttpTaskInfo {
private:
	
	STLString			mUrl;
	CURL*				mEasyHandle;

	USMemStream			mMemStream;
	USByteStream		mByteStream;
	USLeanArray < u8 >	mData;
	
	USStream*			mStream;

	u32					mResponseCode;

	friend class USHttpTask;
	friend class USUrlMgr;

	//----------------------------------------------------------------//
	static void		_printError				( CURLcode error );
	static u32		_writeData				( char* data, u32 n, u32 l, void* s );
	static u32		_writeHeader			( char* data, u32 n, u32 l, void* s );

public:

	//----------------------------------------------------------------//
	void			Clear					();
	void			Finish					();
	void			InitForGet				( cc8* url, cc8* useragent, bool verbose );
	void			InitForPost				( cc8* url, cc8* useragent, const void* buffer, u32 size, bool verbose );
					USHttpTaskInfo			();
					~USHttpTaskInfo			();
};

#endif
#endif
