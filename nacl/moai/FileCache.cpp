
#include "FileCache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ppapi/c/pp_errors.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include <unistd.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

FileHandler* FileHandler::Create ( pp::Instance* instance,
                                       const std::string& url ) {
	return new FileHandler ( instance, url );
}

FileHandler::FileHandler ( pp::Instance* instance,
                             const std::string& path )
    : mInstance(instance),
      mPath(path),
      mFileSystem ( instance, PP_FILESYSTEMTYPE_LOCALPERSISTENT ),
	  mFile ( instance ),
      cc_factory_(this) {

	mCallback = NULL;
	mFileSystemInitialized = false;
}

FileHandler::~FileHandler () {
}

void FileHandler::Init ( ) {

	pp::CompletionCallback cc = cc_factory_.NewCallback ( &FileHandler::OnOpenFileSystem );

	int32_t res = mFileSystem.Open ( 10, cc );

	printf ( "File sys open res = %d\n", res );

	if ( PP_OK_COMPLETIONPENDING != res ) {
		cc.Run ( res );
	}
}

void FileHandler::OnOpenFileSystem ( int32_t result ) {
	printf ( "OnOpenFileSystem\n" );


}

void FileHandler::StartRead ( GetFileCallback callback ) {

	/*mCallback = callback;
	pp::CompletionCallback cc = cc_factory_.NewCallback ( &FileHandler::OnOpen );

	int32_t res = mFile.Open ( mFileRef, PP_FILEOPENFLAG_READ, cc );

	if ( PP_OK_COMPLETIONPENDING != res ) {
		cc.Run ( res );
	}*/
}

void FileHandler::StartWrite ( const char * buffer, int size ) {

	printf ( "start write2\n ");
	//mWriteBuffer = buffer;

	pp::FileRef fileRef ( mFileSystem, mPath.c_str () );

	mWriteSize = size;
	if( size < kBufferSize ) {
		memcpy ( mStaticBuffer, buffer, size );
	}
	else {
		printf( "trying to write file larger than internal buffer" );
		return;
	}

	pp::CompletionCallback cc = cc_factory_.NewCallback ( &FileHandler::OnOpenWrite );

	int32_t res = mFile.Open ( fileRef, PP_FILEOPENFLAG_WRITE | PP_FILEOPENFLAG_TRUNCATE, cc );

}

void FileHandler::OnOpenWrite ( int32_t result ) {

	printf ( "OnOpenWrite\n ");
	if ( result < 0 ) {
		//some type of error hanlder
		printf ( "OnOpenWrite Error %d\n ", result );
		//ReportResultAndDie ( mPath, "pp::FileHandler::Open() failed", false );
	}
	else {
		
		pp::CompletionCallback cc = cc_factory_.NewCallback ( &FileHandler::OnWrite );

		int32_t res = mFile.Write ( 0, mStaticBuffer, mWriteSize,  cc );

		if ( PP_OK_COMPLETIONPENDING != res ) {
			cc.Run ( res );
		}
	}

}
void FileHandler::OnWrite ( int32_t result ) {
	//why would I want a callback on a write finish?
	printf ( "Write success\n" );
}


void FileHandler::OnOpen ( int32_t result ) {

	if ( result < 0 ) {
		//some type of error hanlder
		ReportResultAndDie ( mPath, "pp::FileHandler::Open() failed", false );
	}
	else {
		//success, read
		mCurrentOffset = 0;
		ReadBody ();
	}

}

void FileHandler::OnRead ( int32_t result ) {
	if ( result < 0 ) {
		//error? do something
		ReportResultAndDie( mPath, "Read < 0 ", true );

	} else if (result != 0) {

		int32_t num_bytes = result < kBufferSize ? result : sizeof ( mStaticBuffer );
		mCurrentOffset += num_bytes;

		mHackyFinalBuffer.reserve ( mHackyFinalBuffer.size () + num_bytes );
		mHackyFinalBuffer.insert ( mHackyFinalBuffer.end(), mStaticBuffer, mStaticBuffer + num_bytes );
	
		ReadBody ();
	} else {  
		// result == 0, end of stream
		ReportResultAndDie( mPath, mHackyFinalBuffer, true );
	}
}

void FileHandler::ReadBody () {

  pp::CompletionCallback cc = cc_factory_.NewCallback ( &FileHandler::OnRead );

  int32_t res = mFile.Read ( mCurrentOffset, mStaticBuffer, sizeof ( mStaticBuffer ),  cc );

	if ( PP_OK_COMPLETIONPENDING != res ) {
		cc.Run ( res );
	}
}

void FileHandler::ReportResultAndDie ( const std::string& fname, const std::string& text, bool success ) {
	ReportResult ( fname, text, success );
	delete this;
}

void FileHandler::ReportResult(const std::string& fname,
                                 const std::string& text,
                                 bool success) {
	if ( success ) {
		printf( "GetURLHandler::ReportResult(Ok).\n" );
	}
	else {
		printf( "GetURLHandler::ReportResult(Err). %s\n", text.c_str ());
	}

	fflush ( stdout );

	if ( mInstance ) {

		mCallback ( text.c_str ());
	}
}