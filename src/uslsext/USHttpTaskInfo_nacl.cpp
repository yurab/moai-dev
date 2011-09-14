// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <uslsext/USData.h>
#include <uslsext/USHttpTask.h>
#include <uslsext/USHttpTaskInfo_nacl.h>
#include <uslsext/USUrlMgr.h>

SUPPRESS_EMPTY_FILE_WARNING
#ifdef MOAI_OS_NACL

//----------------------------------------------------------------//
void USHttpTaskInfo::Finish () {
}

//----------------------------------------------------------------//
void USHttpTaskInfo::InitForGet ( cc8* url, cc8* useragent, bool verbose ) {
	UNUSED ( url );
	UNUSED ( useragent );
	UNUSED ( verbose );
}

//----------------------------------------------------------------//
void USHttpTaskInfo::InitForPost ( cc8* url, cc8* useragent, const void* buffer, u32 size, bool verbose ) {
	UNUSED ( url );
	UNUSED ( useragent );
	UNUSED ( buffer );
	UNUSED ( size );
	UNUSED ( verbose );
}

//----------------------------------------------------------------//
USHttpTaskInfo::USHttpTaskInfo () {
}

//----------------------------------------------------------------//
USHttpTaskInfo::~USHttpTaskInfo () {
}

#endif
