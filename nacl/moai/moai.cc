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
#include "MOAIApp.h"

#include "hmac_sha1.h"

namespace {

pthread_t gThreadId;
bool g_swapping = false;
bool g_generate_uid = false;
MOAISocialConnect *g_SocialConnect = NULL;
}

NaClFileSystem *g_FileSystem = NULL;

NaClQueue<std::string> *g_MessageQueue = NULL;
NaClQueue<pp::InputEvent> *g_InputQueue = NULL;

int g_width = 0;
int g_height = 0;
float g_scale = 0.0f;
int g_bInitialized = 0;
bool g_handlingInput;
MoaiInstance *g_instance = NULL;
pp::Core* g_core = NULL;

bool g_toggles[GT_TOTAL];

#include <stdint.h>

extern "C" {

 __inline__ uint64_t rdtsc(void) {
   uint32_t lo, hi;

   __asm__ __volatile__ (      // serialize
   "xorl %%eax,%%eax \n        cpuid"
   ::: "%rax", "%rbx", "%rcx", "%rdx");

   /* We cannot use "=A", since this would use %rax on x86_64 and return only the lower 32bits of the TSC */
   __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));

   return (uint64_t)hi << 32 | lo;

 }
}

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
		WHEEL,
		MOUSE_LEFT,
		MOUSE_MIDDLE,
		MOUSE_RIGHT,
		TOTAL,
	};
}

//----------------------------------------------------------------//
void HandleSocialMessage ( std::string & message );

void NaClHandleInputEvent ( const pp::InputEvent & event ) {
	
	//NACL_LOG( "Enqueue event in engine\n" );
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
			//AJV TODO :(
			AKUEnqueuePointerEvent ( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::POINTER, mouse_event.GetPosition ().x () / g_scale, mouse_event.GetPosition ().y () / g_scale );
			break;
		}
		case PP_INPUTEVENT_TYPE_WHEEL: {
			pp::WheelInputEvent wheel_event ( event );

			NACL_LOG ( "AKUEnqueueCompassEvent\n" );
			AKUEnqueueCompassEvent ( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::WHEEL, wheel_event.GetTicks ().y ());

			break;
		}
		case PP_INPUTEVENT_TYPE_KEYUP:
		case PP_INPUTEVENT_TYPE_KEYDOWN: {

			pp::KeyboardInputEvent keyboard_event ( event );

			bool keyDown = false;
			if( event.GetType() == PP_INPUTEVENT_TYPE_KEYDOWN ) {
				keyDown = true;
			}

			int keycode = keyboard_event.GetKeyCode ();

#if !SHIPPING
			if ( keyDown ) {
				//toggle hack
				if (( keycode >= 49 ) && ( keycode <= ( 49 + 9 ))) {
					int layer = keycode - 49;
					g_toggles [ layer ] = !g_toggles [ layer ];
				}

				if ( keycode == 77 ) { //m
					g_toggles [ GT_MESH ] = !g_toggles [ GT_MESH ];
				}

				if ( keycode == 84 ) { //t
					g_toggles [ GT_TILEDECK ] = !g_toggles [ GT_TILEDECK ];
				}

				if ( keycode == 68) { //d
					g_toggles [ GT_DECK2D ] = !g_toggles [ GT_DECK2D ];
				}

				if ( keycode == 83) { //s
					g_toggles [ GT_DECK2D_2 ] = !g_toggles [ GT_DECK2D_2 ];
				}

				if ( keycode == 76) { //l
					g_toggles [ GT_QUADLIST ] = !g_toggles [ GT_QUADLIST ];
				}
			}
#endif

			AKUEnqueueKeyboardEvent ( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::KEYBOARD, keycode, keyDown );

			break;
		}
		default: {
			NACL_LOG ( "Moai_NaCl: unHandled event %d\n", event.GetType() );
			break;
		}
	}
}
//----------------------------------------------------------------//
void PostMessageThenDeleteMainThread ( void * userData, int32_t result ) {
	char *message  = ( char * ) userData;

	g_instance->PostMessage ( pp::Var ( message ));

	delete [] message;
}

//----------------------------------------------------------------//
void NaClPostMessage ( char *str ) {

	int size = strlen ( str );
	char *message = new char [ size ];
	strcpy ( message, str );

	pp::CompletionCallback cc ( PostMessageThenDeleteMainThread, message );

	g_core->CallOnMainThread ( 0, cc , 0 );
}

//----------------------------------------------------------------//
void FlushMainThread ( void* userData, int32_t result ) {

	glFlush ();

	g_swapping = false;
}

void NaClFlush () {

	//g_swapping = true;

	pp::CompletionCallback cc ( FlushMainThread, g_instance );
	g_core->CallOnMainThread ( 0, cc , 0 );

	//while ( g_swapping ) {

	//	sleep ( 0.0001f );
	//}
}

void GenerateUIDMainThread ( void* userData, int32_t result ) {
	
	g_instance->PostMessage ( "UID" );
}

void NaClGetUID () {

	g_generate_uid = true;

	pp::CompletionCallback cc ( GenerateUIDMainThread, g_instance );
	g_core->CallOnMainThread ( 0, cc , 0 );

	std::string message;
	while ( g_generate_uid ) {

		while ( g_MessageQueue->PopMessage ( message )) {
			HandleSocialMessage ( message );
			MOAIApp::HandleStoreMessage ( message );
		}

		sleep ( 0.0001f );
	}
}

//----------------------------------------------------------------//
void RenderMainThread ( void* userData, int32_t result ) {

	static NaClMoaiTimer fmodTimer ( "Main__Fmod" );
	fmodTimer.Start ();
	AKUFmodUpdate ();
	fmodTimer.FinishAndPrint ();

	g_instance->DrawSelf ();

	g_swapping = false;
}

//----------------------------------------------------------------//
void NaClRender () {

	g_swapping = true;

	static NaClMoaiTimer flushTimer ( "Main__FlushWait" );
	flushTimer.Start ();
	while ( g_instance->GetOpenGLContext ()->flush_pending ()) {
		sleep ( 0.0001f );
	}
	flushTimer.FinishAndPrint ();

	pp::CompletionCallback cc ( RenderMainThread, g_instance );
	g_core->CallOnMainThread ( 0, cc , 0 );

	while ( g_swapping ) {

		sleep ( 0.0001f );
	}
}

//----------------------------------------------------------------//
void InputMainThread ( void* userData, int32_t result ) {

	pp::InputEvent ievent;
	while ( g_InputQueue->PopMessage ( ievent )) {
		NaClHandleInputEvent ( ievent );
	}

	g_handlingInput = false;
}

//----------------------------------------------------------------//
void NaClInput () {

	g_handlingInput = true;

	pp::CompletionCallback cc ( InputMainThread, g_instance );
	g_core->CallOnMainThread ( 0, cc , 0 );

	while ( g_handlingInput ) {

		sleep ( 0.0001f );
	}

	//AKUEnqueueCompassEvent ( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::WHEEL, 0.0f );

}

//================================================================//
// AKU callbacks
//================================================================//

NaClMoaiTimer::NaClMoaiTimer ( const char * _name ) {
	mName = _name;

	mTimer = rdtsc ();
	mClockTimer = USDeviceTime::GetTimeInSeconds ();
}

void NaClMoaiTimer::Start () {
#if ENABLE_NACLPROFILE
	mTimer = rdtsc ();
	mClockTimer = USDeviceTime::GetTimeInSeconds ();
#endif
}

void NaClMoaiTimer::FinishAndPrint () {
#if ENABLE_NACLPROFILE
	//uint64_t time = rdtsc ();
	//NACL_LOG ( "Timer %s: %d\n", mName.c_str (),  time - mTimer );

	double clockTime = USDeviceTime::GetTimeInSeconds ();
	NACL_LOG ( "Clock Timer %s: %f\n", mName.c_str (),  clockTime - mClockTimer );
#endif
}

double NaClMoaiTimer::GetClockTimer () {
	return mClockTimer;
}

void	_AKUEnterFullscreenModeFunc		();
void	_AKUExitFullscreenModeFunc		();
void	_AKUOpenWindowFunc				( const char* title, int width, int height );
void	_AKUStartGameLoopFunc			();

int g_LuaMem = 0;
int g_TexMem = 0;
void* moai_main ( void *_instance ) {

	g_instance = ( MoaiInstance * ) _instance;
	g_FileSystem->Init ();
	NACL_LOG ( "File System Initialized\n" );

	AKURunScript ( "main.lua" );
	AKURunScript ( "config.lua" );
	AKURunScript ( "game.lua" );

	NaClMoaiTimer mainTimer ( "Frame" );
	NaClMoaiTimer logicTimer ( "Logic" );
	NaClMoaiTimer otherTimer ( "Other" );
	NaClMoaiTimer inputTimer ( "Input" );
	NaClMoaiTimer messagesTimer ( "Messages" );
	NaClMoaiTimer gfxTimer ( "Main__" );

	for ( int i = 0; i < GT_TOTAL; ++i ) {
		g_toggles[i] = true;
	}

	NaClGetUID ();

	while ( true ) {

		//double frameTime = USDeviceTime::GetTimeInSeconds ();

		mainTimer.FinishAndPrint ();
#if ENABLE_NACLPROFILE
		NACL_LOG ( "*****************Start Frame***********************\n" );
#endif
		mainTimer.Start ();

		otherTimer.Start ();

		/*lua_State *L = AKUGetLuaState ();
		if ( lua_getgccount ( L ) != g_LuaMem || MOAIGfxDevice::Get ().GetTextureMemoryUsage () != g_TexMem ) {
			g_LuaMem = lua_getgccount ( L );
			g_TexMem = MOAIGfxDevice::Get ().GetTextureMemoryUsage ();
			NACL_LOG ( "****Memory Updated: ****\n**** Lua: %d****\n**** Tex: %d****\n", g_LuaMem, g_TexMem );
		}*/

		otherTimer.FinishAndPrint ();

		//handle messages
		messagesTimer.Start ();
		std::string message;
		while ( g_MessageQueue->PopMessage ( message )) {
			HandleSocialMessage ( message );
			MOAIApp::HandleStoreMessage ( message );
		}
		messagesTimer.FinishAndPrint ();
		
		inputTimer.Start ();
		NaClInput ();
		inputTimer.FinishAndPrint ();

		logicTimer.Start ();
		AKUUpdate ();
		logicTimer.FinishAndPrint ();

		gfxTimer.Start ();
		NaClRender ();
		gfxTimer.FinishAndPrint ();

		/*if ( ( USDeviceTime::GetTimeInSeconds () - frameTime ) < .01 ) {
			sleep ( 0.0001 );
		}*/
	}

	return NULL;
}

//----------------------------------------------------------------//
void _AKUEnterFullscreenModeFunc () {

	NACL_LOG ( "Moai_NaCl: unimplemented _AKUEnterFullscreenModeFunc\n" );
}

//----------------------------------------------------------------//
void _AKUExitFullscreenModeFunc () {

	NACL_LOG ( "Moai_NaCl: unimplemented _AKUExitFullscreenModeFunc\n" );
}

//----------------------------------------------------------------//
void _AKUOpenWindowFunc ( const char* title, int width, int height ) {
	
	NACL_LOG ( "Moai_NaCl: unimplemented _AKUOpenWindowFunc\n" );
}

//----------------------------------------------------------------//
void _AKUStartGameLoopFunc () {

	NACL_LOG ( "Moai_NaCl: unimplemented _AKUStartGameLoopFunc\n" );
}

//----------------------------------------------------------------//
void HandleSocialMessage ( std::string & message ) {

	if ( message.find( "SOCIAL:OnLoginSuccess:" ) != std::string::npos ) {

		//change to queue for bckgrd thread
		NACL_LOG ( "SOCIAL:OnLoginSuccess!\n" );
		g_SocialConnect->OnLoginSuccess ();
	}
	else if ( message.find( "SOCIAL:OnLoginFailed:" ) != std::string::npos ) {

		//change to queue for bckgrd thread
		NACL_LOG ( "SOCIAL:OnLoginFailed!\n" );
		g_SocialConnect->OnLoginFailed ( "", false ) ;
	}
	else if ( message.find( "SOCIAL:OnRequestSuccess:" ) != std::string::npos ) {

		//change to queue for bckgrd thread
		int responseStartIndex =  strlen ( "SOCIAL:OnRequestSuccess:" );
		int responseEndIndex =  message.find( "#" );

		int responseSize = responseEndIndex - responseStartIndex + 1;
		char *responseText = new char [ responseSize ];

		memset ( responseText, 0, responseSize );
		memcpy ( responseText, message.c_str () + responseStartIndex, responseSize - 1 );

		int responseId = atoi ( message.c_str () + responseEndIndex + 1 ); 

		NACL_LOG ( "mSocialConnect->OnRequestSuccess ( %d, %s )\n", responseId, responseText );
		g_SocialConnect->OnRequestSuccess ( responseId, responseText );
		
		delete [] responseText;
	}
	else if ( message.find ( "UID:" ) != std::string::npos ) {

		int responseStartIndex =  strlen ( "UID:" );

		MOAIEnvironment::Get().SetUDID ( message.c_str () + responseStartIndex );

		g_generate_uid = false;
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

	NACL_LOG ( "AKUSocialConnectInit appId %s permissions %d, %p\n", appId, nperms, connector );

	MOAISocialConnect * socialConnect = ( MOAISocialConnect * ) connector;

	g_SocialConnect = socialConnect;

	memset ( socialConnect->mAppId, 0, 128 );
	strcpy ( socialConnect->mAppId, appId );

	pp::CompletionCallback cc ( SocialConnectInitMainThread, socialConnect );

	g_core->CallOnMainThread ( 0, cc , 0 );

}

void SocialConnectLogoutMainThread ( void * userData, int32_t result ) {

	char message[256];
	memset ( message, 0, 256 );
	sprintf ( message, "SOCIAL:logout:" );

	g_instance->PostMessage ( pp::Var ( message ));
}

//----------------------------------------------------------------//
void _AKUSocialConnectLogout()
{
	//AJV no need to 'logout'
	NACL_LOG ( "AKUSocialConnectLogout\n" );

	pp::CompletionCallback cc ( SocialConnectLogoutMainThread, NULL );

	g_core->CallOnMainThread ( 0, cc , 0 );
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
	
	NACL_LOG("Facebook: requesting %s %s\n", httpRequestMethod, requestURL);
	
	moaiView->mFBRequestId = requestId;
	
	[[moaiView mFacebook] requestWithGraphPath:[NSString stringWithUTF8String:requestURL]
									 andParams:params
								 andHttpMethod:[NSString stringWithUTF8String:httpRequestMethod]
								   andDelegate:moaiView];*/
	

	NACL_LOG ( "AKUSocialConnectRequest ID: %d, URL: %s, method: %s\n", requestId, requestURL, httpRequestMethod );

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
	
	//NACL_LOG( "HandleInputEvent\n" );
	g_InputQueue->Push ( event );

	return PP_TRUE;
}

void MoaiInstance::DidChangeFocus (	bool has_focus ) {
	NACL_LOG ( "********************************DidChangeFocus %d\n", has_focus );
}

//----------------------------------------------------------------//
void MoaiInstance::DidChangeView ( const pp::Rect& position, const pp::Rect& clip ) {

	NACL_LOG ( "********************************DidChangeView %d,%d\n", clip.size ().width (), clip.size ().height ());
	if ( g_width == position.size ().width () && g_height == position.size ().height () ) {
		return;
	}

	g_width = position.size ().width ();
	g_height = position.size ().height ();

	//AJV extremely ugly hack to 'detect' unix systems or any other system where the default clock is wrong
	//check out "clock() issues with beta SDK, chrome 15 vs 16" in the native client discuss Google group
	if ( !g_bInitialized ) {


		//running hmac sha1 tests
		/*HMAC_SHA1_CTX   c;

		int keylen = 4;
		unsigned char *key = (unsigned char*)"Jefe";
		int datalen = 28;
		unsigned char *data = (unsigned char*)"what do ya want for nothing?";
		//char *digest = hex2data((unsigned char*)"0xaa4ae5e15272d00e95705637ce8a3b55ed402112", 20);

		unsigned char   md[HMAC_SHA1_DIGEST_LENGTH+1];

		HMAC_SHA1_Init(&c);
		//Send the first 3 bytes of key
		HMAC_SHA1_UpdateKey(&c, key, 3);
		HMAC_SHA1_UpdateKey(&c, &(key[3]), keylen - 3);
		HMAC_SHA1_EndKey(&c);
		HMAC_SHA1_StartMessage(&c);
		//Send the first 7 bytes of data
		HMAC_SHA1_UpdateMessage(&c, data, 7);
		HMAC_SHA1_UpdateMessage(&c, &(data[7]), datalen - 7);

        HMAC_SHA1_EndMessage(&(md[0]),&c);

		md[HMAC_SHA1_DIGEST_LENGTH] = 0;

		printf ( "Hash result: " );
		for (int p = 0; p < HMAC_SHA1_DIGEST_LENGTH; p++)
			printf("%02x", (unsigned char)md[p]);

		printf ( "\n" );*/

		double beforeClock = USDeviceTime::GetTimeInSeconds ();
		double beforeTime = g_core->GetTime ();

		while (( g_core->GetTime () - beforeTime ) < 1.0f ) {
			sleep ( 0.1f );
		}

		double afterClock = USDeviceTime::GetTimeInSeconds ();
		//double afterTime = g_core->GetTime ();

		NACL_LOG ( "clocks per second %f, %f\n", beforeClock, afterClock );
		if (( afterClock - beforeClock ) < 0.1f ) {
			USDeviceTime::SetClocksPerSecond ( 1000 );
		}
		else {
			USDeviceTime::SetClocksPerSecond ( 1000000 );
		}
	
	}
	//AJV End extremely ugly hack

	NACL_LOG ( "resize to %d, %d\n", position.size ().width (), position.size ().height () );

	if (opengl_context == NULL) {
		opengl_context = new OpenGLContext ( this );
	}

	NACL_LOG ( "OpenGLContext %p\n", opengl_context );

	opengl_context->InvalidateContext ( this );
	
	opengl_context->ResizeContext ( position.size ());

	NACL_LOG ( "OpenGLContext MakeContextCurrent\n" );

	if ( !opengl_context->MakeContextCurrent ( this )) {
		printf ( "Error: OpenGLContext failed to MakeContextCurrent\n" );
		g_instance->PostMessage ( pp::Var ( "ALERT:OPENGL" ));
		return;
	}

	NACL_LOG ( "OpenGLContext Created\n" );
	if ( !g_bInitialized ) {

		glClearColor(0,0,0,0);
		glClear(GL_COLOR_BUFFER_BIT);
		glFinish ();
		opengl_context->FlushContext();

		g_FileSystem = new NaClFileSystem ( g_core, this );
		NACL_LOG ( "NaClFileSystem Created\n" );

		g_MessageQueue = new  NaClQueue<std::string> ();
		g_InputQueue = new  NaClQueue<pp::InputEvent> ();
		NACL_LOG ( "NaClQueue Created\n" );

		AKUCreateContext ();
		NACL_LOG ( "AKUContext Created\n" );

		AKUSetInputConfigurationName ( "AKUNaCl" );

		AKUReserveInputDevices			( NaClInputDeviceID::TOTAL );
		AKUSetInputDevice				( NaClInputDeviceID::DEVICE, "device" );
	
		AKUReserveInputDeviceSensors	( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::TOTAL );
		AKUSetInputDeviceKeyboard		( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::KEYBOARD,		"keyboard" );
		AKUSetInputDevicePointer		( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::POINTER,		"pointer" );
		//AJV hijack compass for the wheel
		AKUSetInputDeviceCompass		( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::WHEEL,		"wheel" );

		AKUSetInputDeviceButton			( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::MOUSE_LEFT,	"mouseLeft" );
		AKUSetInputDeviceButton			( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::MOUSE_MIDDLE,	"mouseMiddle" );
		AKUSetInputDeviceButton			( NaClInputDeviceID::DEVICE, NaClInputDeviceSensorID::MOUSE_RIGHT,	"mouseRight" );

		RequestInputEvents ( PP_INPUTEVENT_CLASS_MOUSE );
		RequestInputEvents ( PP_INPUTEVENT_CLASS_KEYBOARD );
		RequestInputEvents ( PP_INPUTEVENT_CLASS_WHEEL );
		NACL_LOG ( "Input Initialized\n" );

		AKUSetFunc_EnterFullscreenMode ( _AKUEnterFullscreenModeFunc );
		AKUSetFunc_ExitFullscreenMode ( _AKUExitFullscreenModeFunc );
		AKUSetFunc_OpenWindow ( _AKUOpenWindowFunc );
		AKUSetFunc_StartGameLoop ( _AKUStartGameLoopFunc );

		AKUSetFunc_SocialConnectInit( _AKUSocialConnectInit );
		AKUSetFunc_SocialConnectLogout( _AKUSocialConnectLogout );
		AKUSetFunc_SocialConnectRequest( _AKUSocialConnectRequest );
		NACL_LOG ( "AKU functions registered Initialized\n" );

		AKUFmodInit ();
		NACL_LOG ( "Fmod Initialized\n" );

		AKUDetectGfxContext ();
		NACL_LOG ( "AKUDetectGfxContext\n" );

		REGISTER_LUA_CLASS ( MOAIApp )

		pthread_create( &gThreadId, NULL, moai_main, g_instance );
		NACL_LOG ( "Main Thread Created\n" );

		g_bInitialized = 1;
	}

	// AJV TODO :(
	MOAIGfxDevice::Get ().SetRealSize ( g_width, g_height );
	g_scale = ( g_width / 1024.0f );
}

//----------------------------------------------------------------//
void MoaiInstance::DrawSelf() {

	if ( !opengl_context->flush_pending () ) {

		static NaClMoaiTimer renderTimer ( "Main_Render" );
		renderTimer.Start ();
		opengl_context->MakeContextCurrent(this);
		
		AKURender ();
		renderTimer.FinishAndPrint ();

		static NaClMoaiTimer finishTimer ( "Main_Finsh" );
		finishTimer.Start ();
		glFinish ();
		finishTimer.FinishAndPrint ();

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
	g_MessageQueue->Push ( message );
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
