// Copyright (c) 2011 The Native Client Authors. All rights reserved.

// Use of this source code is governed by a BSD-style license that can be

// found in the LICENSE file.



#ifndef FILECACHE_H

#define FILECACHE_H



#include <string>

#include "ppapi/cpp/completion_callback.h"

#include "ppapi/cpp/file_system.h"

#include "ppapi/cpp/file_ref.h"

#include "ppapi/cpp/instance.h"

#include "ppapi/cpp/file_io.h"

#include "ppapi/c/ppb_file_io.h"



// GetURLHandler is used to download data from |url|. When download is

// finished or when an error occurs, it posts a message back to the browser

// with the results encoded in the message as a string and self-destroys.

//

// EXAMPLE USAGE:

// GetURLHandler* handler* = GetURLHandler::Create(instance,url);

// handler->Start();



typedef void (* GetFileCallback )( const char *buffer );



class FileHandler {

 public:

  // Creates instance of GetURLHandler on the heap.
  // GetURLHandler objects shall be created only on the heap (they
  // self-destroy when all data is in).
  static FileHandler* Create(pp::Instance* instance_,
                               const std::string& url);

  void Init ( );

  void StartRead ( GetFileCallback callback );

  void StartWrite ( const char * buffer, int size );

 private:
  static const int kBufferSize = 4096;

  FileHandler ( pp::Instance* instance_, const std::string& url );
  ~FileHandler();

  void OnOpenFileSystem ( int32_t result );

  void OnOpenWrite ( int32_t result );

  void OnOpen ( int32_t result );
  void OnWrite ( int32_t result );

  // Callback fo the pp::URLLoader::ReadResponseBody().
  // |result| contains the number of bytes read or an error code.
  // Appends data from this->buffer_ to this->url_response_body_.
  void OnRead(int32_t result);

  // Reads the response body (asynchronously) into this->buffer_.
  // OnRead() will be called when bytes are received or when an error occurs.
  void ReadBody();

  // Post a message back to the browser with the download results.
  void ReportResult(const std::string& fname,
                    const std::string& text,
                    bool success);

  // Post a message back to the browser with the download results and
  // self-destroy.  |this| is no longer valid when this method returns.
  void ReportResultAndDie(const std::string& fname,
                          const std::string& text,
                          bool success);

  GetFileCallback mCallback;

  pp::Instance* mInstance;

  std::string mPath;

  pp::FileSystem mFileSystem; 

  pp::FileIO mFile;

  const char *mWriteBuffer;
  int mWriteSize;
  char mStaticBuffer [ kBufferSize ];  // buffer for pp::URLLoader::ReadResponseBody().

  int mCurrentOffset;
  bool mFileSystemInitialized;

  std::string mHackyFinalBuffer;  // Contains downloaded data.
  pp::CompletionCallbackFactory<FileHandler> cc_factory_;

  FileHandler(const FileHandler&);
  void operator=(const FileHandler&);
};

#endif  // EXAMPLES_GETURL_GETURL_HANDLER_H_

