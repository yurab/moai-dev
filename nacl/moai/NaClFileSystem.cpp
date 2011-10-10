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
	
	printf ( "NaClFileSystem::fopen %s, mode %s \n", path, mode );

	//reading modes
	if ( mode[0] == 'r' ) {

		//TODO, check if file is cached
		pp::CompletionCallback cc ( RequestURLMainThread, newFile );

		if ( !mCore->IsMainThread ()) {
			mCore->CallOnMainThread ( 0, cc , 0 );
		}
		else {
			printf( "ERROR: Cannot open file for reading on main thread\n" );
			RequestURLMainThread ( newFile, 0 );
			newFile->mIsHttpLoaded = true;
		}

		while ( !newFile->mIsHttpLoaded ) {

			sleep ( 0.01f );
		}

	} 
	else if ( mode[0] == 'w' ) {

		pp::CompletionCallback cc ( OpenFileMainThread, newFile );

		if ( !mCore->IsMainThread ()) {
			mCore->CallOnMainThread ( 0, cc , 0 );
		}
		else {
			printf( "ERROR: Cannot open file for writing on main thread\n" );
			OpenFileMainThread ( newFile, 0 );
			newFile->mIsFileOpen = true;
		}

		while ( !newFile->mIsFileOpen ) {

			sleep ( 0.01f );
		}

		printf ("opened file for writing.\n" );
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
void NaClFileSystem::OpenFileMainThread ( void * userData, int32_t result ) {
  
	NaClFile * file = static_cast < NaClFile * > ( userData );
	
	pp::CompletionCallback cc ( OpenFileCallback, file );

	file->mFileRef = new pp::FileRef ( mSingletonInstance->mFileSystem, file->mPath );
	file->mFileIO = new pp::FileIO ( mSingletonInstance->mInstance );

	file->mFileIO->Open ( *( file->mFileRef ), PP_FILEOPENFLAG_WRITE | PP_FILEOPENFLAG_CREATE | PP_FILEOPENFLAG_TRUNCATE , cc );
}

//----------------------------------------------------------------//
void NaClFileSystem::OpenFileCallback ( void * userData, int32_t result ) {
  
	NaClFile * file = static_cast < NaClFile * > ( userData );
	
	file->mIsFileOpen = true;
}

//----------------------------------------------------------------//
int NaClFileSystem::stat ( const char *path, struct stat *buf ) {

	NaClFile * newFile = new NaClFile ();
	newFile->mPath = path;

	//TODO, check if file is cached
	pp::CompletionCallback cc ( RequestURLStatsMainThread, newFile );

	if ( !mCore->IsMainThread ()) {
		mCore->CallOnMainThread ( 0, cc , 0 );
	}
	else {
		printf( "ERROR: Cannot load files on main thread\n" );
		RequestURLStatsMainThread ( newFile, 0 );
		newFile->mIsHttpLoaded = true;
	}

	while ( !newFile->mIsHttpLoaded ) {

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
size_t NaClFileSystem::fread ( void *ptr, size_t size_of_elements, size_t number_of_elements, NaClFile *file ) {

	if( file && file->mIsHttpLoaded ) {

		int readSize = size_of_elements * number_of_elements;
		int remainingSize = file->mSize - file->mOffset;

		if ( readSize <= remainingSize ) {
			memcpy ( ptr, file->mData + file->mOffset, readSize );
			file->mOffset += readSize;
			return readSize;
		}
		else {
			memcpy ( ptr, file->mData + file->mOffset, remainingSize );
			file->mOffset += remainingSize;
			return remainingSize;
		}
	}
	else {
		printf ( "NaClFileSystem::fread - invalid file" );
	}

	return 0;
}

size_t NaClFileSystem::fwrite ( const void *ptr, size_t size, size_t count, NaClFile *file ) {

	if( file && file->mIsFileOpen ) {

		file->mIsFileLocked = true;
		file->mData = ( char* ) ptr;
		file->mSize = size *  count;

		pp::CompletionCallback cc ( WriteFileMainThread, file );
		
		if ( !mCore->IsMainThread ()) {
			mCore->CallOnMainThread ( 0, cc , 0 );
		}
		else {
			printf( "ERROR: Cannot write files on main thread\n" );
			WriteFileMainThread ( file, 0 );
			file->mIsFileLocked = false;
		}

		while ( file->mIsFileLocked ) {
			sleep ( 0.01f );
		}

		file->mOffset += size *  count;
	}

	return 0;
}

//----------------------------------------------------------------//
void NaClFileSystem::WriteFileMainThread ( void * userData, int32_t result ) {

	NaClFile * file = static_cast < NaClFile * > ( userData );

	pp::CompletionCallback cc ( WriteFileDone, file );
	file->mFileIO->Write ( file->mOffset, file->mData, file->mSize, cc );
}

void NaClFileSystem::WriteFileDone ( void * userData, int32_t result )  {

	NaClFile * file = static_cast < NaClFile * > ( userData );

	file->mIsFileLocked = false;
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

	file->mIsHttpLoaded = true;
}

//----------------------------------------------------------------//
NaClFileSystem * NaClFileSystem::Get () {

	return mSingletonInstance;
}

NaClFile::NaClFile () {
	mIsHttpLoaded = false;
	mSize = 0;
	mData = NULL;
	mOffset = 0;
	mExists = false;
	mFileRef = NULL;
	mFileIO = NULL;
}

NaClFile::~NaClFile () {
	mIsHttpLoaded = false;
	mSize = 0;
	if ( mData ) {
		delete [] mData;
	}

	if ( mFileRef ) {
		delete mFileRef;
	}

	if ( mFileIO ) {
		delete mFileIO;
	}

	mOffset = 0;
	mExists = false;
}
