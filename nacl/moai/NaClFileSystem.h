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

	bool mIsHttpLoaded;

	bool mIsFileOpen;
	bool mIsFileLocked;

	char * mData;
	int mSize;
	int mOffset;
	bool mExists;

	pp::FileRef *mFileRef;
	pp::FileIO *mFileIO;
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

	size_t fread ( void *ptr, size_t size, size_t count, NaClFile *a_file );
	size_t fwrite ( const void *ptr, size_t size, size_t count, NaClFile *a_file );

	int stat ( const char *path, struct stat *buf );

private:
	
	//main thread callback functions, C API since callback factory does not work on bg thread

	//URL
	static void RequestURLMainThread ( void* userData, int32_t result );
	static void RequestURLStatsMainThread ( void * userData, int32_t result );

	//File
	static void OpenFileMainThread ( void * userData, int32_t result );
	static void OpenFileCallback ( void * userData, int32_t result );

	static void WriteFileMainThread ( void * userData, int32_t result );
	static void WriteFileDone ( void * userData, int32_t result );

	//pp Filesystem
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
