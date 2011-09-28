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

	int32_t res = Get ()->mFileSystem.Open ( 1024, cc );

	if ( PP_OK_COMPLETIONPENDING != res ) {
		cc.Run ( res );
	}
}

//----------------------------------------------------------------//
void NaClFileSystem::OpenFileSystemCallback ( int32_t result ) {

	mFileSystemOpened = true;
}

//----------------------------------------------------------------//
NaClFile * NaClFileSystem::fopen ( const char * path, const char *mode ) {

	//int modeLen = strlen ( mode );

	NaClFile * newFile = new NaClFile ();
	newFile->mPath = path;
	
	printf ( "NaClFileSystem::fopen %s \n", path );

	//reading modes
	if ( mode[0] == 'r' ) {

		//TODO, check if file is cached
		pp::CompletionCallback cc ( RequestURLMainThread, newFile );
		mCore->CallOnMainThread ( 0, cc , 0 );

		while ( !newFile->mHttpLoaded ) {

			sleep ( 0.01f );
		}

	} 
	else if ( mode[0] == 'w' ) {
		//TODO writing

	}
	else {
		printf ( "NaClFileSystem::fopen - Unsupported mode %s\n", mode );
	}

	return newFile;
}

//----------------------------------------------------------------//
void NaClFileSystem::RequestURLMainThread ( void * userData, int32_t result ) {
  
	NaClFile * file = static_cast < NaClFile * > ( userData );
	GetURLHandler* handler = GetURLHandler::Create( Get ()->mInstance, file->mPath );

	if (handler != NULL) {

		handler->Start( HttpLoaded, file );
	}
}

//----------------------------------------------------------------//
int NaClFileSystem::stat ( const char *path, struct stat *buf ) {

	NaClFile * newFile = new NaClFile ();
	newFile->mPath = path;

	//TODO, check if file is cached
	pp::CompletionCallback cc ( RequestURLStatsMainThread, newFile );
	mCore->CallOnMainThread ( 0, cc , 0 );

	while ( !newFile->mHttpLoaded ) {

		sleep ( 0.01f );
	}

	if ( newFile->mExists ) {
		delete newFile;
		return 0;
	}
	else {
		delete newFile;
		return -1;
	}
}

//----------------------------------------------------------------//
void NaClFileSystem::RequestURLStatsMainThread ( void * userData, int32_t result ) {
  
	NaClFile * file = static_cast < NaClFile * > ( userData );

	GetURLHandler* handler = GetURLHandler::Create( Get ()->mInstance, file->mPath );
	handler->SetMethod ( GetURLHandler::HEAD );

	if (handler != NULL) {

		handler->Start( HttpLoaded, file );
	}

}

//----------------------------------------------------------------//
int NaClFileSystem::fclose ( NaClFile *file ) {

	printf ( "NaClFileSystem::fclose %s \n\n", file->mPath );
	delete file;
	return 0;
}

//----------------------------------------------------------------//
size_t NaClFileSystem::fread ( void *ptr, size_t size_of_elements, size_t number_of_elements, NaClFile *a_file ) {

	if( a_file && a_file->mHttpLoaded ) {

		int readSize = size_of_elements * number_of_elements;
		int remainingSize = a_file->mSize - a_file->mOffset;

		if ( readSize <= remainingSize ) {
			memcpy ( ptr, a_file->mData + a_file->mOffset, readSize );
			a_file->mOffset += readSize;
			return readSize;
		}
		else {
			memcpy ( ptr, a_file->mData + a_file->mOffset, remainingSize );
			a_file->mOffset += remainingSize;
			return remainingSize;
		}
	}
	else {
		printf ( "NaClFileSystem::fread - invalid file" );
	}

	return 0;
}

size_t fwrite ( const void *ptr, size_t size_of_elements, size_t number_of_elements, NaClFile *a_file ) {

	printf ( "NaClFileSystem::fwrite - unimplemented" );
	return 0;
}

//----------------------------------------------------------------//
void NaClFileSystem::HttpLoaded ( void *_file, const char *buffer, int32_t size ) {

	NaClFile *file = static_cast < NaClFile * > ( _file );

	if ( file->mExists && size ) {

		printf ( "NaClFileSystem::HttpLoaded file %s, size: %d\n", file->mPath, size );

		file->mSize = size;
		file->mData = new char [ size ];

		memcpy ( file->mData, buffer, size );
	}

	file->mHttpLoaded = true;
}

//----------------------------------------------------------------//
NaClFileSystem * NaClFileSystem::Get () {

	return mSingletonInstance;
}

NaClFile::NaClFile () {
	mHttpLoaded = false;
	mSize = 0;
	mData = NULL;
	mOffset = 0;
	mExists = false;
}

NaClFile::~NaClFile () {
	mHttpLoaded = false;
	mSize = 0;
	if ( mData ) {
		delete [] mData;
	}
	mOffset = 0;
	mExists = false;
}
