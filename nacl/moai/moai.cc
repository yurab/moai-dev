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
#include "moaicore/pch.h"
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

#include "moaicore/MOAIGfxDevice.h"
#include "moaicore/MOAISocialConnect.h"

namespace {

pthread_t gThreadId;
bool g_swapping = false;
MOAISocialConnect *g_SocialConnect = NULL;
}

NaClFileSystem *g_FileSystem = NULL;
NaClMessageQueue *g_MessageQueue = NULL;

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

//----------------------------------------------------------------//
void HandleSocialMessage ( std::string & message );

//----------------------------------------------------------------//
void RenderMainThread ( void* userData, int32_t result ) {

	AKUFmodUpdate ();

	g_instance->DrawSelf ();

	g_swapping = false;
}

//----------------------------------------------------------------//
void NaClRender () {

	g_swapping = true;

	pp::CompletionCallback cc ( RenderMainThread, g_instance );
	g_core->CallOnMainThread ( 0, cc , 0 );

	while ( g_swapping ) {

		sleep ( 0.01f );
	}
}

//----------------------------------------------------------------//
int NaClMessageQueue::PopMessage ( std::string &message ) {

	pthread_mutex_lock( &mutex );

	int result = 0;

	if ( num > 0) {

		result = 1;
		message = messages [ tail ];
		++tail;

		if ( tail >=  NaClMessageQueue::kMaxMessages) {
			tail -=  NaClMessageQueue::kMaxMessages;
		}
		--num;
	}

	pthread_mutex_unlock( &mutex );

	return result;
}

//----------------------------------------------------------------//
void NaClMessageQueue::PushMessage ( std::string &message ) {

	pthread_mutex_lock( &mutex );

	if ( num >= NaClMessageQueue::kMaxMessages ) {
		printf ("ERROR: g_MessageQueue, kMaxMessages (%d) exceeded\n", NaClMessageQueue::kMaxMessages );
	} 
	else {
		int head = ( tail + num) % NaClMessageQueue::kMaxMessages;

		 messages [ head ] = message;
		++num;

		if ( num >= NaClMessageQueue::kMaxMessages )  {
			 num -= NaClMessageQueue::kMaxMessages;
		}

		//pthread_cond_signal( & condvar );
	}

	pthread_mutex_unlock( &mutex );
}

//================================================================//
// AKU callbacks
//================================================================//

void	_AKUEnterFullscreenModeFunc		();
void	_AKUExitFullscreenModeFunc		();
void	_AKUOpenWindowFunc				( const char* title, int width, int height );
void	_AKUStartGameLoopFunc			();

int g_LuaMem = 0;
int g_TexMem = 0;
void* moai_main ( void *_instance ) {

	g_instance = ( MoaiInstance * ) _instance;
	g_FileSystem->Init ();

	AKURunScript ( "main.lua" );
	AKURunScript ( "config.lua" );
	AKURunScript ( "game.lua" );

	while ( true ) {

		lua_State *L = AKUGetLuaState ();
		if ( lua_getgccount ( L ) != g_LuaMem || MOAIGfxDevice::Get ().GetTextureMemoryUsage () != g_TexMem ) {
			g_LuaMem = lua_getgccount ( L );
			g_TexMem = MOAIGfxDevice::Get ().GetTextureMemoryUsage ();
			printf ( "****Memory Updated: ****\n**** Lua: %d****\n**** Tex: %d****\n", g_LuaMem, g_TexMem );
		}

		//handle messages
		std::string message;
		while ( g_MessageQueue->PopMessage ( message )) {
			HandleSocialMessage ( message );
		}

		AKUUpdate ();

		NaClRender ();

	}

	return NULL;
}

//----------------------------------------------------------------//
void _AKUEnterFullscreenModeFunc () {

	printf ( "Moai_NaCl: unimplemented _AKUEnterFullscreenModeFunc\n" );
}

//----------------------------------------------------------------//
void _AKUExitFullscreenModeFunc () {

	printf ( "Moai_NaCl: unimplemented _AKUExitFullscreenModeFunc\n" );
}

//----------------------------------------------------------------//
void _AKUOpenWindowFunc ( const char* title, int width, int height ) {
	
	printf ( "Moai_NaCl: unimplemented _AKUOpenWindowFunc\n" );
}

//----------------------------------------------------------------//
void _AKUStartGameLoopFunc () {

	printf ( "Moai_NaCl: unimplemented _AKUStartGameLoopFunc\n" );
}

//----------------------------------------------------------------//
void HandleSocialMessage ( std::string & message ) {

	if ( message.find( "SOCIAL:OnLoginSuccess:" ) != std::string::npos ) {

		//change to queue for bckgrd thread
		printf ( "SOCIAL:OnLoginSuccess!\n" );
		g_SocialConnect->OnLoginSuccess ();
	}
	else if ( message.find( "SOCIAL:OnLoginFailed:" ) != std::string::npos ) {

		//change to queue for bckgrd thread
		printf ( "SOCIAL:OnLoginFailed!\n" );
		g_SocialConnect->OnLoginFailed ( "", false ) ;
	}
	else if ( message.find( "SOCIAL:OnRequestSuccess:" ) != std::string::npos ) {

		//change to queue for bckgrd thread
		int responseStartIndex =  strlen ( "SOCIAL:OnRequestSuccess:" );
		int responseEndIndex =  message.find( "&" );

		int responseSize = responseEndIndex - responseStartIndex + 1;
		char *responseText = new char [ responseSize ];

		memset ( responseText, 0, responseSize );
		memcpy ( responseText, message.c_str () + responseStartIndex, responseSize - 1 );

		int responseId = atoi ( message.c_str () + responseEndIndex + 1 ); 

		printf ( "mSocialConnect->OnRequestSuccess ( %d, %s )\n", responseId, responseText );
		g_SocialConnect->OnRequestSuccess ( responseId, responseText );
		
		delete [] responseText;
	}
}

//----------------------------------------------------------------//
void SocialConnectInitMainThread ( void * userData, int32_t result ) {
	MOAISocialConnect * socialConnect = ( MOAISocialConnect * ) userData;

	char message[256];
	memset ( message, 0, 256 );
	sprintf ( message, "SOCIAL:init:%s", socialConnect->mAppId );

	g_instance->PostMessage ( pp::Var ( message ));
}
//----------------------------------------------------------------//
void _AKUSocialConnectInit(const char* appId, int nperms, const char** perms, void *connector)
{
	/*
	AJV TODO - permissions, actually use apId
	NSMutableArray* permissions = nil;
	if( nperms > 0 )
	{
		permissions = [NSMutableArray array];
		for( int i = 0; i < nperms; i++ )
			[permissions addObject:[NSString stringWithUTF8String:perms[i]]];
	}
	
	[moaiView fbInit:_appId withPermissions:permissions connector:(MOAISocialConnect*)connector];*/

	printf ( "AKUSocialConnectInit appId %s permissions %d, %p\n", appId, nperms, connector );

	MOAISocialConnect * socialConnect = ( MOAISocialConnect * ) connector;

	g_SocialConnect = socialConnect;

	memset ( socialConnect->mAppId, 0, 128 );
	strcpy ( socialConnect->mAppId, appId );

	pp::CompletionCallback cc ( SocialConnectInitMainThread, socialConnect );

	g_core->CallOnMainThread ( 0, cc , 0 );

}

//----------------------------------------------------------------//
void _AKUSocialConnectLogout()
{
	//AJV no need to 'logout'
	printf ( "AKUSocialConnectLogout\n" );
}		

struct SocialConnectRequest {
	std::string mGraphURL;
	int id;
};

void SocialConnectRequestMainThread ( void * userData, int32_t result ) {
	SocialConnectRequest *req  = ( SocialConnectRequest * ) userData;

	char message[512];
	memset ( message, 0, 512 );
	sprintf ( message, "SOCIAL:request:%s,%d", req->mGraphURL.c_str (), req->id );

	delete req;

	g_instance->PostMessage ( pp::Var ( message ));
}
//----------------------------------------------------------------//
void _AKUSocialConnectRequest(int requestId, const char* requestURL, const char* httpRequestMethod, int nargs, const char** args)
{
	/*
	AJV TODO - using params
	NSMutableDictionary *params = [NSMutableDictionary dictionary];
	for( int i = 0; i < nargs; i++ )
	{
		[params setObject:[NSString stringWithUTF8String:args[2*i]]
				   forKey:[NSString stringWithUTF8String:args[2*i+1]]];
	}
	
	printf("Facebook: requesting %s %s\n", httpRequestMethod, requestURL);
	
	moaiView->mFBRequestId = requestId;
	
	[[moaiView mFacebook] requestWithGraphPath:[NSString stringWithUTF8String:requestURL]
									 andParams:params
								 andHttpMethod:[NSString stringWithUTF8String:httpRequestMethod]
								   andDelegate:moaiView];*/
	

	printf ( "AKUSocialConnectRequest ID: %d, URL: %s, method: %s\n", requestId, requestURL, httpRequestMethod );

	SocialConnectRequest *req = new SocialConnectRequest ();
	req->mGraphURL = requestURL;
	req->id = requestId;

	pp::CompletionCallback cc ( SocialConnectRequestMainThread, req );

	g_core->CallOnMainThread ( 0, cc , 0 );
}

//----------------------------------------------------------------//
MoaiInstance::~MoaiInstance() {
}

//----------------------------------------------------------------//
bool MoaiInstance::Init ( uint32_t /* argc */, const char* /* argn */[], const char* /* argv */[] ) {

	g_instance = this;

	opengl_context = NULL;

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
			printf ( "Moai_NaCl: unHandled event %d\n", event.GetType() );
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

	//AJV extremely ugly hack to 'detect' unix systems or any other system where the default clock is wrong
	//check out "clock() issues with beta SDK, chrome 15 vs 16" in the native client discuss Google group
	//NOTE: After talking with google, stay tuned, new timer coming soon!
	double beforeClock = USDeviceTime::GetTimeInSeconds ();
	double beforeTime = g_core->GetTime ();

	while (( g_core->GetTime () - beforeTime ) < 1.0f ) {
		sleep ( 0.1f );
	}

	double afterClock = USDeviceTime::GetTimeInSeconds ();
	double afterTime = g_core->GetTime ();

	printf ( "clocks per second %f, %f\n", beforeClock, afterClock );
	if (( afterClock - beforeClock ) < 0.1f ) {
		USDeviceTime::SetClocksPerSecond ( 1000 );
	}
	//AJV End extremely ugly hack

	printf ( "resize to %d, %d\n", position.size ().width (), position.size ().height () );

	if (opengl_context == NULL) {
		opengl_context = new OpenGLContext ( this );
	}

	opengl_context->InvalidateContext ( this );
	
	opengl_context->ResizeContext ( position.size ());

	if ( !opengl_context->MakeContextCurrent ( this )) {
		return;
	}

	g_FileSystem = new NaClFileSystem ( g_core, this );
	g_MessageQueue = new NaClMessageQueue ();

	AKUCreateContext ();

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

	AKUSetFunc_SocialConnectInit( _AKUSocialConnectInit );
	AKUSetFunc_SocialConnectLogout( _AKUSocialConnectLogout );
	AKUSetFunc_SocialConnectRequest( _AKUSocialConnectRequest );

	AKUFmodInit ();

	AKUDetectGfxContext ();

	pthread_create( &gThreadId, NULL, moai_main, g_instance );	
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

	//AJV send to queue and proccess on background thread ( due to thread safety issues / file I/O )
	g_MessageQueue->PushMessage ( message );
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
