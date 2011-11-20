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

	printf ( "USHttpTaskInfo::HttpLoaded status? %d, size %d, pointer %p, data %s\n", handler->GetStatusCode (), size, taskInfo, buffer );

	/*taskInfo->mByteStream.SetBuffer ( const_cast<char *> ( buffer ), size );
	taskInfo->mByteStream.SetLength ( size );
	taskInfo->mStream = &taskInfo->mByteStream;*/

	taskInfo->mStream->WriteBytes ( buffer, size );

	taskInfo->mReady = true;
}

//----------------------------------------------------------------//
void USHttpTaskInfo::HttpGetMainThread ( void* userData, int32_t result ) {

	USHttpTaskInfo * taskInfo = static_cast < USHttpTaskInfo * > ( userData );

	GetURLHandler* handler = GetURLHandler::Create( g_instance, taskInfo->mUrl );
	
	if (handler != NULL) {

		printf ( "Getting.... %s\n", taskInfo->mUrl.c_str ());
		//handler->SetMethod ( GetURLHandler::GET );
		handler->SetUserData ( taskInfo );

		handler->Start( HttpLoaded );
	}

	taskInfo->mLock = false;
}


//----------------------------------------------------------------//
void USHttpTaskInfo::HttpPostMainThread ( void* userData, int32_t result ) {

	USHttpTaskInfo * taskInfo = static_cast < USHttpTaskInfo * > ( userData );

	GetURLHandler* handler = GetURLHandler::Create( g_instance, taskInfo->mUrl );
	
	if (handler != NULL) {

		handler->SetMethod ( GetURLHandler::POST );
		handler->SetUserData ( taskInfo );

		NACL_LOG ( "\nSet Body: %s\n", taskInfo->mTempBufferToCopy );
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

	NACL_LOG ("USHttpTaskInfo::Finish %p\n", this );
	if ( this->mStream == &this->mMemStream ) {
	
		u32 size = this->mMemStream.GetLength ();
		NACL_LOG ("USHttpTaskInfo::Finish get size %d\n", size );
		
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

	this->mReady = false;
	this->mUrl = url;
	this->mLock = true;

	printf ( "get %s\n", url );
	pp::CompletionCallback cc ( HttpGetMainThread, this );
	g_core->CallOnMainThread ( 0, cc , 0 );

	while ( this->mLock ) {
		sleep ( 0.0001f );
	}

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
		sleep ( 0.0001f );
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
