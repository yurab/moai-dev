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
int MOAIChartBoost::_loadInterstitial ( lua_State* L ) {
	
	MOAILuaState state ( L );

	cc8* location = NULL; //lua_tostring ( state, 1 ); At the moment, the ChartBoost caching API for Android does not support locations.

	JNI_GET_ENV ( jvm, env );
	
	JNI_GET_JSTRING ( location, jlocation );

	jclass chartboost = env->FindClass ( "com/ziplinegames/moai/MoaiChartBoost" );
    if ( chartboost == NULL ) {

		USLog::Print ( "MOAIChartBoost: Unable to find java class %s", "com/ziplinegames/moai/MoaiChartBoost" );
    } else {

    	jmethodID loadInterstitial = env->GetStaticMethodID ( chartboost, "loadInterstitial", "(Ljava/lang/String;)V" );
    	if ( loadInterstitial == NULL ) {

			USLog::Print ( "MOAIChartBoost: Unable to find static java method %s", "loadInterstitial" );
    	} else {

			env->CallStaticVoidMethod ( chartboost, loadInterstitial, jlocation );			
		}
	}
			
	return 0;
}

//----------------------------------------------------------------//
int MOAIChartBoost::_setListener ( lua_State* L ) {
	
	MOAILuaState state ( L );
	
	u32 idx = state.GetValue < u32 >( 1, TOTAL );

	if ( idx < TOTAL ) {
		
		MOAIChartBoost::Get ().mListeners [ idx ].SetStrongRef ( state, 2 );
	}
	
	return 0;
}

//----------------------------------------------------------------//
int MOAIChartBoost::_showInterstitial ( lua_State* L ) {
	
	MOAILuaState state ( L );

	cc8* location = NULL; //lua_tostring ( state, 1 ); At the moment, the ChartBoost caching API for Android does not support locations.

	JNI_GET_ENV ( jvm, env );
	
	JNI_GET_JSTRING ( location, jlocation );

	jclass chartboost = env->FindClass ( "com/ziplinegames/moai/MoaiChartBoost" );
    if ( chartboost == NULL ) {

		USLog::Print ( "MOAIChartBoost: Unable to find java class %s", "com/ziplinegames/moai/MoaiChartBoost" );
    } else {

    	jmethodID showInterstitial = env->GetStaticMethodID ( chartboost, "showInterstitial", "(Ljava/lang/String;)Z" );
    	if ( showInterstitial == NULL ) {

			USLog::Print ( "MOAIChartBoost: Unable to find static java method %s", "showInterstitial" );
    	} else {

			jboolean jsuccess = ( jboolean )env->CallStaticBooleanMethod ( chartboost, showInterstitial, jlocation );				

			lua_pushboolean ( state, jsuccess );

			return 1;
		}
	}

	lua_pushboolean ( state, false );

	return 1;
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

	state.SetField ( -1, "INTERSTITIAL_LOAD_FAILED",	( u32 )INTERSTITIAL_LOAD_FAILED );
	state.SetField ( -1, "INTERSTITIAL_DISMISSED", 		( u32 )INTERSTITIAL_DISMISSED );

	luaL_Reg regTable [] = {
		{ "init",				_init },
		{ "loadInterstitial",	_loadInterstitial },
		{ "setListener",		_setListener },
		{ "showInterstitial",	_showInterstitial },
		{ NULL, NULL }
	};

	luaL_register ( state, 0, regTable );
}

//----------------------------------------------------------------//
void MOAIChartBoost::NotifyInterstitialDismissed () {	
	
	MOAILuaRef& callback = this->mListeners [ INTERSTITIAL_DISMISSED ];
	
	if ( callback ) {
		
		MOAILuaStateHandle state = callback.GetSelf ();
		
		state.DebugCall ( 0, 0 );
	}
}

//----------------------------------------------------------------//
void MOAIChartBoost::NotifyInterstitialLoadFailed () {	
	
	MOAILuaRef& callback = this->mListeners [ INTERSTITIAL_LOAD_FAILED ];
	
	if ( callback ) {
		
		MOAILuaStateHandle state = callback.GetSelf ();
		
		state.DebugCall ( 0, 0 );
	}
}

//================================================================//
// ChartBoost JNI methods
//================================================================//

//----------------------------------------------------------------//
extern "C" void Java_com_ziplinegames_moai_MoaiChartBoost_AKUNotifyChartBoostInterstitialDismissed ( JNIEnv* env, jclass obj ) {

	MOAIChartBoost::Get ().NotifyInterstitialDismissed ();
}

//----------------------------------------------------------------//
extern "C" void Java_com_ziplinegames_moai_MoaiChartBoost_AKUNotifyChartBoostInterstitialLoadFailed ( JNIEnv* env, jclass obj ) {

	MOAIChartBoost::Get ().NotifyInterstitialLoadFailed ();
}

#endif