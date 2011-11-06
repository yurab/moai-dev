// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef	MOAIDECK2DSHADER_FSH_H
#define	MOAIDECK2DSHADER_FSH_H

#include "NaClFileSystem.h"
#include "ppapi/cpp/input_event.h"

class OpenGLContext;
//----------------------------------------------------------------//
class MoaiInstance : public pp::Instance {
	public:
	/// The constructor creates the plugin-side instance.
	/// @param[in] instance the handle to the browser-side plugin instance.
	explicit MoaiInstance(PP_Instance instance) : pp::Instance(instance)
	{
	}
	virtual ~MoaiInstance();

	OpenGLContext *opengl_context;

	OpenGLContext *GetOpenGLContext () { return opengl_context; }

	void DrawSelf();

	bool Init ( uint32_t /* argc */, const char* /* argn */[], const char* /* argv */[] );

	virtual void DidChangeView ( const pp::Rect& position, const pp::Rect& clip );
	virtual void DidChangeFocus ( bool has_focus );

	virtual void HandleMessage ( const pp::Var& var_message );

	virtual bool HandleInputEvent ( const pp::InputEvent & event );
};

//async
void NaClPostMessage ( char *str );

template <class T>
class NaClQueue {
public:
	pthread_mutex_t mutex;
	int tail;
	int num;

	static const int kMaxMessages = 100;

	T messages [ kMaxMessages ];

	//----------------------------------------------------------------//
	void Push ( const T &message ) {

		pthread_mutex_lock( &mutex );

		if ( num >= NaClQueue::kMaxMessages ) {
			printf ("ERROR: g_MessageQueue, kMaxMessages (%d) exceeded\n", NaClQueue::kMaxMessages );
		} 
		else {
			int head = ( tail + num) % NaClQueue::kMaxMessages;

			 messages [ head ] = message;
			++num;

			if ( num >= NaClQueue::kMaxMessages )  {
				 num -= NaClQueue::kMaxMessages;
			}

			//pthread_cond_signal( & condvar );
		}

		pthread_mutex_unlock( &mutex );
	}

	int PopMessage ( T &message ) {

		pthread_mutex_lock( &mutex );

		int result = 0;

		if ( num > 0) {

			result = 1;
			message = messages [ tail ];
			++tail;

			if ( tail >=  NaClQueue::kMaxMessages) {
				tail -=  NaClQueue::kMaxMessages;
			}
			--num;
		}

		pthread_mutex_unlock( &mutex );

		return result;
	}
};

class NaClMoaiTimer {
public:
	std::string mName;

	uint64_t mTimer;
	double mClockTimer;

	NaClMoaiTimer ( const char * name );
	~NaClMoaiTimer () {}

	void Start ();
	double GetClockTimer ();
	void FinishAndPrint ();
};

#define SHIPPING 0
#define ENABLE_NACLPROFILE 0

#if SHIPPING
#define NACL_LOG(...)
#else
//#define NACL_LOG(...) printf("Time:%f:",USDeviceTime::GetTimeInSeconds ());printf(__VA_ARGS__)
#define NACL_LOG(...) printf(__VA_ARGS__)
#endif

extern NaClQueue<std::string> *g_MessageQueue;
extern NaClFileSystem *g_FileSystem;
extern pp::Core* g_core;
extern MoaiInstance *g_instance;

#endif
