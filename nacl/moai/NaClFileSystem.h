#ifndef NACLFILESYSTEM_H
#define NACLFILESYSTEM_H

#include <string>
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/file_system.h"
#include "ppapi/cpp/file_ref.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/file_io.h"
#include "ppapi/c/ppb_file_io.h"

class NaClFile {
public:
	NaClFile ();
	~NaClFile ();

	const char * mPath;
	bool mHttpLoaded;
	char * mData;
	int mSize;
	int mOffset;
	bool mExists;
};


class NaClFileSystem {
public:

	NaClFileSystem ( pp::Core* core, pp::Instance *instance );
	~NaClFileSystem ();

	void Init ();

	static NaClFileSystem * Get ();

	//TODO - cache file to HD on open
	NaClFile * fopen ( const char * path, const char *mode );

	int fclose ( NaClFile *file );

	size_t fread ( void *ptr, size_t size_of_elements, size_t number_of_elements, NaClFile *a_file );
	size_t fwrite ( const void *ptr, size_t size_of_elements, size_t number_of_elements, NaClFile *a_file );

	int stat ( const char *path, struct stat *buf );

private:
	
	//main thread callback functions, C API since callback factory does not work on bg thread
	static void RequestURLMainThread ( void* userData, int32_t result );
	static void RequestURLStatsMainThread ( void * userData, int32_t result );

	static void OpenFileSystemMainThread ( void* userData, int32_t result );

	void OpenFileSystemCallback ( int32_t result );

	pp::CompletionCallbackFactory<NaClFileSystem> mCCFactory;

	pp::Core* mCore;
	pp::Instance *mInstance;
	
	bool mFileSystemOpened;

	pp::FileSystem mFileSystem;

	static NaClFileSystem *mSingletonInstance;

	//TODO move to my own url handler
	static void HttpLoaded ( void *_instance, const char *buffer, int32_t size );

	//filemap? for caching
	NaClFile mCurrentFile;
};

#endif
