// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef USHTTPTASKINFO_NACL_H
#define USHTTPTASKINFO_NACL_H
#ifdef MOAI_OS_NACL
#include "geturl_handler.h"
//================================================================//
// USHttpTaskInfo
//================================================================//
class USHttpTaskInfo {
private:
	STLString			mUrl;
	STLString			mBody;

	USMemStream			mMemStream;
	USByteStream		mByteStream;
	USLeanArray < u8 >	mData;
	
	USStream*			mStream;
	//USMemStream			mStream;

	u32					mResponseCode;
	bool				mReady;

	bool				mLock;

	const void *		mTempBufferToCopy;
	int					mTempBufferToCopySize;

	friend class USHttpTask;
	friend class USUrlMgr;

	static void HttpLoaded ( GetURLHandler *handler, const char *buffer, int32_t size );
	static void HttpPostMainThread ( void* userData, int32_t result );
	static void HttpGetMainThread ( void* userData, int32_t result );

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
