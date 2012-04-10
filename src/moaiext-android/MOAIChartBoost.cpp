// Copyright (c) 2010-2011 Zipline Games, Inc. All Rights Reserved.
// http://getmoai.com

#ifndef DISABLE_CHARTBOOST

#include "pch.h"

#include <jni.h>

#include <moaiext-android/moaiext-jni.h>
#include <moaiext-android/MOAIChartBoost.h>

extern JavaVM* jvm;

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
/**	@name	init
	@text	Initialize ChartBoost.
	
	@in		string	appId			Available in ChartBoost dashboard settings.
	@in 	string	appSignature	Available in ChartBoost dashboard settings.
	@out 	nil
*/
int MOAIChartBoost::_init ( lua_State* L ) {
	
	MOAILuaState state ( L );

	cc8* identifier = lua_tostring ( state, 1 );
	cc8* signature = lua_tostring ( state, 2 );

	JNI_GET_ENV ( jvm, env );
	
	JNI_GET_JSTRING ( identifier, jidentifier );
	JNI_GET_JSTRING ( signature, jsignature );

	jclass chartboost = env->FindClass ( "com/ziplinegames/moai/MoaiChartBoost" );
    if ( chartboost == NULL ) {

		USLog::Print ( "MOAIChartBoost: Unable to find java class %s", "com/ziplinegames/moai/MoaiChartBoost" );
    } else {

    	jmethodID init = env->GetStaticMethodID ( chartboost, "init", "(Ljava/lang/String;Ljava/lang/String;)V" );
    	if ( init == NULL ) {

			USLog::Print ( "MOAIChartBoost: Unable to find static java method %s", "init" );
    	} else {

			env->CallStaticVoidMethod ( chartboost, init, jidentifier, jsignature );				
		}
	}
			
	return 0;
}

//----------------------------------------------------------------//
/**	@name	showInterstitial
	@text	Request an interstitial ad display.
	
	@opt	string	locationId		Optional location ID.
	@out 	nil
*/
int MOAIChartBoost::_showInterstitial ( lua_State* L ) {
	
	MOAILuaState state ( L );

	cc8* location = lua_tostring ( state, 1 );

	JNI_GET_ENV ( jvm, env );
	
	JNI_GET_JSTRING ( location, jlocation );

	jclass chartboost = env->FindClass ( "com/ziplinegames/moai/MoaiChartBoost" );
    if ( chartboost == NULL ) {

		USLog::Print ( "MOAIChartBoost: Unable to find java class %s", "com/ziplinegames/moai/MoaiChartBoost" );
    } else {

    	jmethodID showInterstitial = env->GetStaticMethodID ( chartboost, "showInterstitial", "(Ljava/lang/String;)V" );
    	if ( showInterstitial == NULL ) {

			USLog::Print ( "MOAIChartBoost: Unable to find static java method %s", "showInterstitial" );
    	} else {

			env->CallStaticVoidMethod ( chartboost, showInterstitial, jlocation );				
		}
	}
			
	return 0;
}

//================================================================//
// MOAIChartBoost
//================================================================//

//----------------------------------------------------------------//
MOAIChartBoost::MOAIChartBoost () {

	RTTI_SINGLE ( MOAILuaObject )	
}

//----------------------------------------------------------------//
MOAIChartBoost::~MOAIChartBoost () {

}

//----------------------------------------------------------------//
void MOAIChartBoost::RegisterLuaClass ( MOAILuaState& state ) {

	luaL_Reg regTable [] = {
		{ "init",				_init },
		{ "showInterstitial",	_showInterstitial },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

#endif