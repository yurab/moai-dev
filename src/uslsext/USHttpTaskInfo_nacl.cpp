// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <uslsext/USData.h>
#include <uslsext/USHttpTask.h>
#include <uslsext/USHttpTaskInfo_nacl.h>
#include <uslsext/USUrlMgr.h>

SUPPRESS_EMPTY_FILE_WARNING
#ifdef MOAI_OS_NACL

#include "moai_nacl.h"
//----------------------------------------------------------------//
void USHttpTaskInfo::HttpLoaded ( GetURLHandler *handler, const char *buffer, int32_t size ) {

	USHttpTaskInfo *taskInfo = static_cast < USHttpTaskInfo * > ( handler->GetUserData ());
	taskInfo->mResponseCode = handler->GetStatusCode ();

	printf ( "USHttpTaskInfo::HttpLoaded status %d, size %d, data %s\n", handler->GetStatusCode (), size, buffer );

	taskInfo->mByteStream.SetBuffer ( const_cast<char *> ( buffer ), size );
	taskInfo->mByteStream.SetLength ( size );
	taskInfo->mStream = &taskInfo->mByteStream;

	taskInfo->mReady = true;
}

//----------------------------------------------------------------//
void USHttpTaskInfo::HttpPostMainThread ( void* userData, int32_t result ) {

	USHttpTaskInfo * taskInfo = static_cast < USHttpTaskInfo * > ( userData );

	GetURLHandler* handler = GetURLHandler::Create( g_instance, taskInfo->mUrl );
	
	if (handler != NULL) {

		handler->SetMethod ( GetURLHandler::POST );
		handler->SetUserData ( taskInfo );

		printf ( "Set Body: %s\n", taskInfo->mTempBufferToCopy );
		handler->SetBody ( taskInfo->mTempBufferToCopy, taskInfo->mTempBufferToCopySize );

		handler->Start( HttpLoaded );
	}

	taskInfo->mLock = false;
}

//----------------------------------------------------------------//
void USHttpTaskInfo::Clear () {
	this->mUrl.clear ();
	this->mData.Clear ();
	this->mReady = false;
	this->mResponseCode = 0;
}

//----------------------------------------------------------------//
void USHttpTaskInfo::Finish () {

	if ( this->mStream == &this->mMemStream ) {
	
		u32 size = this->mMemStream.GetLength ();
		
		if ( size ) {
			this->mData.Init ( size );
			this->mStream->Seek ( 0, SEEK_SET );
			this->mStream->ReadBytes ( this->mData, size );
		}
		this->mMemStream.Clear ();
	}
}

//----------------------------------------------------------------//
void USHttpTaskInfo::InitForGet ( cc8* url, cc8* useragent, bool verbose ) {
	UNUSED ( url );
	UNUSED ( useragent );
	UNUSED ( verbose );

	this->Clear ();

	//AJV TODO GET
	printf ( "USHttpTaskInfo::InitForGet ( %s, %s, verbose )\n", url, useragent );

	this->mUrl = url;
}

//----------------------------------------------------------------//
void USHttpTaskInfo::InitForPost ( cc8* url, cc8* useragent, const void* buffer, u32 size, bool verbose ) {
	UNUSED ( url );
	UNUSED ( useragent );
	UNUSED ( buffer );
	UNUSED ( size );
	UNUSED ( verbose );

	this->Clear ();

	this->mReady = false;
	this->mUrl = url;
	this->mTempBufferToCopy = buffer;
	this->mTempBufferToCopySize = size;
	this->mLock = true;

	pp::CompletionCallback cc ( HttpPostMainThread, this );
	g_core->CallOnMainThread ( 0, cc , 0 );

	while ( this->mLock ) {
		sleep ( 0.001f );
	}

	//printf ( "USHttpTaskInfo::InitForPost ( %s, %s, %s, %d, verbose )\n", url, useragent, buffer, size );
}

//----------------------------------------------------------------//
USHttpTaskInfo::USHttpTaskInfo () {

	this->mStream = &this->mMemStream;
}

//----------------------------------------------------------------//
USHttpTaskInfo::~USHttpTaskInfo () {

	this->Clear ();
}

#endif
