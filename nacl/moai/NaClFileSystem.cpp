#include <cstdio>
#include <string>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

#include "ppapi/cpp/file_ref.h"

#include "NaClFileSystem.h"
#include "geturl_handler.h"

NaClFileSystem *NaClFileSystem::mSingletonInstance = NULL;

//----------------------------------------------------------------//
NaClFileSystem::~NaClFileSystem () {
	mCore = NULL;
	mInstance = NULL;
}

//----------------------------------------------------------------//
NaClFileSystem::NaClFileSystem ( pp::Core* core, pp::Instance *instance ) :
	mCCFactory ( this ),
	mFileSystem ( instance, PP_FILESYSTEMTYPE_LOCALPERSISTENT )
{
	mCore = core;
	mInstance = instance;
	mSingletonInstance = this;
	mFileSystemOpened = false;
}

//----------------------------------------------------------------//
void NaClFileSystem::Init () {

	pp::CompletionCallback cc ( OpenFileSystemMainThread, this );
	mCore->CallOnMainThread ( 0, cc , 0 );

	//block while waiting for filesys
	while ( !mFileSystemOpened ) {

		sleep ( 0.01f );
	}
}

//----------------------------------------------------------------//
void NaClFileSystem::OpenFileSystemMainThread ( void* userData, int32_t result ) {

	pp::CompletionCallback cc = Get ()->mCCFactory.NewCallback ( &NaClFileSystem::OpenFileSystemCallback );
	Get ()->mFileSystem.Open ( 1024, cc );
}

//----------------------------------------------------------------//
void NaClFileSystem::OpenFileSystemCallback ( int32_t result ) {

	mFileSystemOpened = true;
}

//----------------------------------------------------------------//
NaClFile * NaClFileSystem::fopen ( const char * path, const char *mode ) {

	int modeLen = strlen ( mode );

	NaClFile * newFile = new NaClFile ();
	newFile->mPath = path;
	newFile->mHttpLoaded = false;

	//reading modes
	if ( mode[0] == 'r' ) {

		//TODO, check if file is cached
		pp::CompletionCallback cc ( RequestURLMainThread, newFile );
		mCore->CallOnMainThread ( 0, cc , 0 );

		printf ( "Block On URL loading...\n" );

		while ( !newFile->mHttpLoaded ) {

			sleep ( 0.01f );
		}
	} 
	else if ( mode[0] == 'w' ) {
		//TODO writing

	}
	else {
		printf ( "fopen: Unsupported mode %s\n", mode );
	}

	return newFile;
}

//----------------------------------------------------------------//
int NaClFileSystem::fclose ( NaClFile *file ) {

	delete file;
	return 0;
}

//----------------------------------------------------------------//
void NaClFileSystem::RequestURLMainThread ( void * userData, int32_t result ) {
  
	NaClFile * file = static_cast < NaClFile * > ( userData );
	GetURLHandler* handler = GetURLHandler::Create( Get ()->mInstance, file->mPath );

	if (handler != NULL) {

		printf ( "with file1 %p\n", userData );
		handler->Start( HttpLoaded, userData );
	}
}

//----------------------------------------------------------------//
void NaClFileSystem::HttpLoaded ( void *_file, const char *buffer, int32_t size ) {

	NaClFile *file = static_cast < NaClFile * > ( _file );

	printf ( "with file2 %p\n", _file );
	//printf ( "Buffer test size: %d, buffer: %s", size, buffer );

	file->mData.reserve ( size );
	file->mData.insert ( file->mData.end(), buffer, buffer + size );
	file->mHttpLoaded = true;

	//printf ( "String data %s", file->mData.c_str ());
}

//----------------------------------------------------------------//
NaClFileSystem * NaClFileSystem::Get () {

	return mSingletonInstance;
}
