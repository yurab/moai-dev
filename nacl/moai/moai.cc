/// @file moai.cc

#include <cstdio>
#include <string>
#include <string.h>
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"

#include "ppapi/cpp/file_ref.h"

#include "geturl_handler.h"
#include "NaClFileSystem.h"
#include <unistd.h>
#include <stdio.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "zipfs/zipfs.h"

#include "ppapi/gles2/gl2ext_ppapi.h"
#include <GLES2/gl2.h>

namespace {

const char* const kHelloString = "hello";
const char* const kReplyString = "hello from NaCl";

pthread_t gThreadId;
int g_stage = 0;
pp::Core* g_core = NULL;
NaClFileSystem *g_FileSystem = NULL;
pp::Instance *g_instance = NULL;

}

void luaDoFile ( const char *path ) {

	NaClFile *file = g_FileSystem->fopen ( path, "r" );
	printf ( "loaded %s\n", file->mData.c_str ());

	lua_State *L = lua_open ();
	luaL_openlibs ( L );

	luaL_loadbuffer ( L, file->mData.c_str (), strlen ( file->mData.c_str () ), "line" );
	lua_pcall ( L, 0, 0, 0 );

	lua_close ( L );

	g_FileSystem->fclose ( file );

	printf ( "all done\n" );
}

void* moai_main ( void *_instance ) {

	printf ( "begin main\n" );
	g_instance = ( pp::Instance * ) _instance;
	g_FileSystem->Init ();

	zipfs_init ();

	while ( true ) {

		switch ( g_stage ) {
		case 0:
			{
				luaDoFile ( "test.lua" );
				g_stage = 1;
			}
			break;
		case 1:
			break;
		case 2:
			break;
		}
		sleep ( 0.01f );
	}

	return NULL;
}

//----------------------------------------------------------------//
class MoaiInstance : public pp::Instance {
	public:
	/// The constructor creates the plugin-side instance.
	/// @param[in] instance the handle to the browser-side plugin instance.
	explicit MoaiInstance(PP_Instance instance) : pp::Instance(instance)
	{
	}
	virtual ~MoaiInstance() {}

	/// Handler for messages coming in from the browser via postMessage().  The
	/// @a var_message can contain anything: a JSON string; a string that encodes
	/// method names and arguments; etc.  For example, you could use
	/// JSON.stringify in the browser to create a message that contains a method
	/// name and some parameters, something like this:
	///   var json_message = JSON.stringify({ "myMethod" : "3.14159" });
	///   nacl_module.postMessage(json_message);
	/// On receipt of this message in @a var_message, you could parse the JSON to
	/// retrieve the method name, match it to a function call, and then call it
	/// with the parameter.
	/// @param[in] var_message The message posted by the browser.

	// Update the graphics context to the new size, and regenerate |pixel_buffer_|
	// to fit the new size as well.

	bool Init ( uint32_t /* argc */, const char* /* argn */[], const char* /* argv */[] ) {

		g_FileSystem = new NaClFileSystem ( g_core, this );
		pthread_create( &gThreadId, NULL, moai_main, this );

		return true;
	}

	virtual void DidChangeView ( const pp::Rect& position, const pp::Rect& clip ) {

		printf ( "did change view \n" );

		if ( opengl_context_ == NULL ) {
			opengl_context_.reset( new OpenGLContext ( this ));
		}
		 
		opengl_context_->InvalidateContext ( this );

		if ( !opengl_context_->MakeContextCurrent ( this )) {
			return;
		}
	}

	virtual void HandleMessage ( const pp::Var& var_message ) {
  
		if ( !var_message.is_string ()) {
			return;
		}

		std::string message = var_message.AsString ();

		pp::Var var_reply;

		if ( message == kHelloString ) {
		 
		}
	}
};

//----------------------------------------------------------------//
class MoaiModule : public pp::Module {
 public:
  MoaiModule () : pp::Module () {}
  virtual ~MoaiModule () {
	  glTerminatePPAPI();
  }

  virtual bool Init () {

	  g_core = core ();
	  return glInitializePPAPI ( get_browser_interface ()) == GL_TRUE;
  }

  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return new MoaiInstance ( instance );
  }
};

namespace pp {

Module* CreateModule () {
  return new MoaiModule ();
}
}
