// Copyright (c) 2011 The Native Client Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "opengl_context.h"

#include <pthread.h>
#include <stdio.h>
#include "ppapi/gles2/gl2ext_ppapi.h"

namespace {
// This is called by the brower when the 3D context has been flushed to the
// browser window.
void FlushCallback(void* data, int32_t result) {
	//printf ( "OpenGLContext FlushCallback \n" );
  static_cast<OpenGLContext*>(data)->set_flush_pending(false);
}
}  // namespace


OpenGLContext::OpenGLContext(pp::Instance* instance)
    : pp::Graphics3DClient_Dev(instance),
      flush_pending_(false) {
  pp::Module* module = pp::Module::Get();
  assert(module);
  gles2_interface_ = static_cast<const struct PPB_OpenGLES2_Dev*>(
      module->GetBrowserInterface(PPB_OPENGLES2_DEV_INTERFACE));
  assert(gles2_interface_);
}

OpenGLContext::~OpenGLContext() {
  glSetCurrentContextPPAPI(0);
}

bool OpenGLContext::MakeContextCurrent ( pp::Instance* instance ) {
  
	if ( instance == NULL ) {
		printf ( "OpenGLContext null instance\n" );
		glSetCurrentContextPPAPI(0);
		return false;
	}

  //printf ( "OpenGLContext MakeContextCurrent \n" );
  // Lazily create the Pepper context.
  if ( context_.is_null ()) {

    context_ = pp::Context3D_Dev(*instance, 0, pp::Context3D_Dev(), NULL);

	if ( context_.is_null ()) {
		printf ( "OpenGLContext null context_\n" );
		glSetCurrentContextPPAPI ( 0 );
		return false;
	}

    surface_ = pp::Surface3D_Dev(*instance, 0, NULL);
    context_.BindSurfaces(surface_, surface_);
    instance->BindGraphics(surface_);
  }

  glSetCurrentContextPPAPI ( context_.pp_resource ());

  return true;
}

void OpenGLContext::InvalidateContext(pp::Instance* instance) {
  
	if (instance == NULL)
    return;

  // Unbind the existing surface and re-bind to null surfaces.
  instance->BindGraphics(pp::Surface3D_Dev());
  context_.BindSurfaces(pp::Surface3D_Dev(), pp::Surface3D_Dev());

  glSetCurrentContextPPAPI(0);

}

void OpenGLContext::FlushContext() {
  if (flush_pending()) {
    // A flush is pending so do nothing; just drop this flush on the floor.
    return;
  }

  //printf ( "FlushContext \n" );
  set_flush_pending(true);
  surface_.SwapBuffers(pp::CompletionCallback(&FlushCallback, this));
}
