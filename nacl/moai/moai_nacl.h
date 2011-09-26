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

	virtual void HandleMessage ( const pp::Var& var_message );

	virtual bool HandleInputEvent ( const pp::InputEvent & event );
};

extern NaClFileSystem *g_FileSystem;
extern bool g_blockOnMainThread;
extern pp::Core* g_core;
extern MoaiInstance *g_instance;

#endif
