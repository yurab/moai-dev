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
#include "opengl_context.h"
#include "moai_nacl.h"

#include <unistd.h>
#include <stdio.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "zipfs/zipfs.h"
#include <aku/AKU.h>

#include "ppapi/gles2/gl2ext_ppapi.h"
#include <GLES2/gl2.h>

#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/size.h"
#include "ppapi/cpp/var.h"

#include <aku/AKU-fmod.h>

namespace {

const char* const kHelloString = "hello";
const char* const kReplyString = "hello from NaCl";

pthread_t gThreadId;


bool g_swapping = false;

}

NaClFileSystem *g_FileSystem = NULL;
MoaiInstance *g_instance = NULL;
bool g_blockOnMainThread = false;

pp::Core* g_core = NULL;

namespace NaClInputDeviceID {
	enum {
		DEVICE = 0,
		TOTAL,
	};
}

namespace NaClInputDeviceSensorID {
	enum {
		KEYBOARD = 0,
		POINTER,
		MOUSE_LEFT,
		MOUSE_MIDDLE,
		MOUSE_RIGHT,
		TOTAL,
	};
}

void luaDoFile ( const char *path ) {

	NaClFile *file = g_FileSystem->fopen ( path, "r" );
	printf ( "loaded %s\n", file->mData );

	lua_State *L = AKUGetLuaState ();
	//luaL_openlibs ( L );

	luaL_loadbuffer ( L, file->mData, file->mSize, "line" );
	lua_pcall ( L, 0, 0, 0 );

	printf ( "loading done\n" );
	//lua_close ( L );

	g_FileSystem->fclose ( file );
}

void RenderMainThread ( void* userData, int32_t result );

//----------------------------------------------------------------//
void NaClRender () {

	g_swapping = true;

	pp::CompletionCallback cc ( RenderMainThread, g_instance );
	g_core->CallOnMainThread ( 0, cc , 0 );

	//block while waiting for filesys
	while ( g_swapping ) {

		sleep ( 0.01f );
	}
}

//----------------------------------------------------------------//
void RenderMainThread ( void* userData, int32_t result ) {

	AKUFmodUpdate ();

	g_instance->DrawSelf ();
	g_swapping = false;
}

//================================================================//
// AKU callbacks
//================================================================//

void	_AKUEnterFullscreenModeFunc		();
void	_AKUExitFullscreenModeFunc		();
void	_AKUOpenWindowFunc				( const char* title, int width, int height );
void	_AKUStartGameLoopFunc			();

void* moai_main ( void *_instance ) {

	printf ( "begin main\n" );
	g_instance = ( MoaiInstance * ) _instance;
	g_FileSystem->Init ();

	AKURunScript ( "main.lua" );
	AKURunScript ( "config.lua" );
	AKURunScript ( "game.lua" );

	printf ( "Done Running Script\n" );

	while ( true ) {

		//printf ( "Update\n" );

		AKUUpdate ();

		//printf ( "Render\n" );

		NaClRender ();

		//printf ( "Render Done\n" );
	}

	return NULL;
}

//----------------------------------------------------------------//
void _AKUEnterFullscreenModeFunc () {

	//wut?
	printf ( " _AKUEnterFullscreenModeFunc\n" );
}

//----------------------------------------------------------------//
void _AKUExitFullscreenModeFunc () {

	//wut?
	printf ( " _AKUExitFullscreenModeFunc\n" );
}

//----------------------------------------------------------------//
void _AKUOpenWindowFunc ( const char* title, int width, int height ) {
	
	printf ( " _AKUOpenWindowFunc\n" );
	//AKUDetectGfxContext ();
}

//----------------------------------------------------------------//
void _AKUStartGameLoopFunc () {

	printf ( " _AKUStartGameLoopFunc\n" );
}

//----------------------------------------------------------------//
MoaiInstance::~MoaiInstance() {
}

//----------------------------------------------------------------//
bool MoaiInstance::Init ( uint32_t /* argc */, const char* /* argn */[], const char* /* argv */[] ) {

	g_FileSystem = new NaClFileSystem ( g_core, this );

	g_instance = this;

	AKUCreateContext ();

	opengl_context = NULL;

	AKUSetInputConfigurationName ( "AKUNaCl" );

	AKUReserveInputDevices			( NaClInputDeviceID::TOTAL );
	AKUSetInputDevice				( NaClInputDeviceID::DEVICE, "device" );
	
	AKUReserveInputDeviceSensors	( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::TOTAL );
	AKUSetInputDevicePointer		( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::KEYBOARD,		"keyboard" );
	AKUSetInputDevicePointer		( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::POINTER,		"pointer" );
	AKUSetInputDeviceButton			( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::MOUSE_LEFT,	"mouseLeft" );
	AKUSetInputDeviceButton			( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::MOUSE_MIDDLE,	"mouseMiddle" );
	AKUSetInputDeviceButton			( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::MOUSE_RIGHT,	"mouseRight" );

	RequestInputEvents ( PP_INPUTEVENT_CLASS_MOUSE );
	RequestInputEvents ( PP_INPUTEVENT_CLASS_KEYBOARD );

	AKUSetFunc_EnterFullscreenMode ( _AKUEnterFullscreenModeFunc );
	AKUSetFunc_ExitFullscreenMode ( _AKUExitFullscreenModeFunc );
	AKUSetFunc_OpenWindow ( _AKUOpenWindowFunc );
	AKUSetFunc_StartGameLoop ( _AKUStartGameLoopFunc );

	AKUFmodInit ();

	//AJV TODO - should go to run script
	//needs to block for context creation as well
	pthread_create( &gThreadId, NULL, moai_main, g_instance );

	return true;
}

//----------------------------------------------------------------//
bool MoaiInstance::HandleInputEvent	( const pp::InputEvent & event ) {
	
	switch ( event.GetType() ) {
		case PP_INPUTEVENT_TYPE_MOUSEDOWN:
		case PP_INPUTEVENT_TYPE_MOUSEUP: {

			pp::MouseInputEvent mouse_event ( event );

			bool mouseDown = false;
			if( event.GetType() == PP_INPUTEVENT_TYPE_MOUSEDOWN ) {
				mouseDown = true;
			}

			switch ( mouse_event.GetButton() ) {
				case PP_INPUTEVENT_MOUSEBUTTON_LEFT:
					AKUEnqueueButtonEvent ( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::MOUSE_LEFT, mouseDown );
					break;
				case PP_INPUTEVENT_MOUSEBUTTON_RIGHT:
					AKUEnqueueButtonEvent ( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::MOUSE_RIGHT, mouseDown );
					break;
				case PP_INPUTEVENT_MOUSEBUTTON_MIDDLE:
					AKUEnqueueButtonEvent ( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::MOUSE_MIDDLE, mouseDown );
					break;
				default:
					break;
			}
			break;
		}
		case PP_INPUTEVENT_TYPE_MOUSEMOVE: {
			pp::MouseInputEvent mouse_event ( event );
			AKUEnqueuePointerEvent ( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::POINTER, mouse_event.GetPosition ().x (), mouse_event.GetPosition ().y () );
			break;
		}
		case PP_INPUTEVENT_TYPE_KEYDOWN: 
		case PP_INPUTEVENT_TYPE_KEYUP: {

			pp::KeyboardInputEvent keyboard_event ( event );

			bool keyDown = false;
			if( event.GetType() == PP_INPUTEVENT_TYPE_KEYDOWN ) {
				keyDown = true;
			}

			int keycode = keyboard_event.GetKeyCode ();
			AKUEnqueueKeyboardEvent ( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::KEYBOARD, keycode, keyDown );

			break;
		}
		default:
			printf ( "NaclHost: unHandled event %d\n", event.GetType() );
			return false;
	}

	return PP_TRUE;
}

int g_width = 0;
int g_height = 0;

//----------------------------------------------------------------//
void MoaiInstance::DidChangeView ( const pp::Rect& position, const pp::Rect& clip ) {

	if ( g_width == position.size ().width () && g_height == position.size ().height () ) {
		return;
	}

	g_width = position.size ().width ();
	g_height = position.size ().height ();

	if ( opengl_context == NULL ) {

		printf ( "new OpenGLContext\n" );
		opengl_context = new OpenGLContext ( this );
	}
		 
	opengl_context->InvalidateContext ( this );

	if ( !opengl_context->MakeContextCurrent ( this )) {
		return;
	}

	AKUDetectGfxContext ();

	printf ( "resize to %d, %d\n", position.size ().width (), position.size ().height () );
}

//----------------------------------------------------------------//
void MoaiInstance::DrawSelf() {

  if ( !opengl_context->flush_pending () ) {

	  opengl_context->MakeContextCurrent(this);

	  AKURender ();

	  opengl_context->FlushContext();
  }

}

//----------------------------------------------------------------//
void MoaiInstance::HandleMessage ( const pp::Var& var_message ) {
  
	if ( !var_message.is_string ()) {
		return;
	}

	std::string message = var_message.AsString ();

	pp::Var var_reply;

	if ( message == kHelloString ) {
		 
	}
}

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
