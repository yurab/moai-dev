// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#include "pch.h"

#include <uslsext/USHttpTask.h>
#include <uslsext/USHttpTaskInfo_nacl.h>
#include <uslsext/USUrlMgr.h>

SUPPRESS_EMPTY_FILE_WARNING
#ifdef MOAI_OS_NACL

//================================================================//
// USUrlMgr
//================================================================//

//----------------------------------------------------------------//
void USUrlMgr::AddHandle ( USHttpTask& task ) {
}

//----------------------------------------------------------------//
bool USUrlMgr::More () {

	return false;
}

//----------------------------------------------------------------//
void USUrlMgr::Process () {
}

//----------------------------------------------------------------//
USUrlMgr::USUrlMgr () {
}

//----------------------------------------------------------------//
USUrlMgr::~USUrlMgr () {
}

#endif
