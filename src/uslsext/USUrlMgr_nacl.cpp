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

static bool sMore = false;
static STLList < USHttpTask* > sTasks;

//----------------------------------------------------------------//
void USUrlMgr::AddHandle ( USHttpTask& task ) {
	
	sTasks.push_back ( &task );
}

//----------------------------------------------------------------//
bool USUrlMgr::More () {

	return false;
}

//----------------------------------------------------------------//
void USUrlMgr::Process () {

	for( STLList < USHttpTask* >::iterator iter = sTasks.begin(); iter != sTasks.end(); ++iter )
	{
		if(( *iter )->GetInfo ()->mReady ) {

			( *iter )->Finish ();

			STLList < USHttpTask* >::iterator old_iter = iter;
			old_iter--;
			sTasks.erase ( iter );
			iter = old_iter;
		}
	}
}

//----------------------------------------------------------------//
USUrlMgr::USUrlMgr () {
}

//----------------------------------------------------------------//
USUrlMgr::~USUrlMgr () {
}

#endif
