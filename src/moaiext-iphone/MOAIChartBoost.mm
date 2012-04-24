//----------------------------------------------------------------//
// Copyright (c) 2010-2011 Zipline Games, Inc. 
// All Rights Reserved. 
// http://getmoai.com
//----------------------------------------------------------------//

#ifndef DISABLE_CHARTBOOST

#import <moaiext-iphone/MOAIChartBoost.h>

//================================================================//
// lua
//================================================================//

//----------------------------------------------------------------//
int MOAIChartBoost::_init ( lua_State* L ) {
	
	MOAILuaState state ( L );

	cc8* identifier = lua_tostring ( state, 1 );
	cc8* signature = lua_tostring ( state, 2 );
	
	[[ ChartBoost sharedChartBoost ] setAppId:[ NSString stringWithUTF8String:identifier ]];
	[[ ChartBoost sharedChartBoost ] setAppSignature:[ NSString stringWithUTF8String:signature ]];
	[[ ChartBoost sharedChartBoost ] setDelegate:MOAIChartBoost::Get ().mDelegate ];
	[[ ChartBoost sharedChartBoost ] startSession ];
	
	return 0;
}

//----------------------------------------------------------------//
int MOAIChartBoost::_loadInterstitial ( lua_State* L ) {
	
	MOAILuaState state ( L );

	// At the moment, to keep parity with Android, don't allow locations.
	// cc8* location = lua_tostring ( state, 1 );
	// 
	// if ( location != nil ) {
	// 	
	// 	[[ ChartBoost sharedChartBoost ] cacheInterstitial:[ NSString stringWithUTF8String:location ]];
	// } else {
	// 	
		[[ ChartBoost sharedChartBoost ] cacheInterstitial ];
	// }
			
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

	// At the moment, to keep parity with Android, don't allow locations.
	// cc8* location = lua_tostring ( state, 1 );
	// 
	// if ( location != nil ) {
	// 	
	// 	if ([[ ChartBoost sharedChartBoost ] hasCachedInterstitial:[ NSString stringWithUTF8String:location ]]) {
	// 		
	// 		[[ ChartBoost sharedChartBoost ] showInterstitial:[ NSString stringWithUTF8String:location ]];
	// 		
	// 		lua_pushboolean ( state, true );
	// 		
	// 		return 1;
	// 	}
	// } else {
		
		if ( [[ ChartBoost sharedChartBoost ] hasCachedInterstitial ]) {
			
			[[ ChartBoost sharedChartBoost ] showInterstitial ];

			lua_pushboolean ( state, true );
			
			return 1;
		}
	// }
			
	lua_pushboolean ( state, false );

	return 1;
}

//================================================================//
// MOAIChartBoost
//================================================================//

//----------------------------------------------------------------//
MOAIChartBoost::MOAIChartBoost () {

	RTTI_SINGLE ( MOAILuaObject )	

	mDelegate = [[ MoaiChartBoostDelegate alloc ] init ];
}

//----------------------------------------------------------------//
MOAIChartBoost::~MOAIChartBoost () {

	[ mDelegate release ];
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
// MoaiChartBoostDelegate
//================================================================//
@implementation MoaiChartBoostDelegate

	//================================================================//
	#pragma mark -
	#pragma mark Protocol MoaiChartBoostDelegate
	//================================================================//

	- ( BOOL ) shouldRequestInterstitial {
		
		return YES;
	}

	- ( void ) didFailToLoadInterstitial {
		
		MOAIChartBoost::Get ().NotifyInterstitialLoadFailed ();
	}

	- ( BOOL ) shouldDisplayInterstitial:( UIView * )interstitialView {
		
		return YES;
	}

	- ( void ) didDismissInterstitial:( UIView * )interstitialView {
		
		MOAIChartBoost::Get ().NotifyInterstitialDismissed ();
	}

	- ( BOOL ) shouldDisplayMoreApps:( UIView * )moreAppsView {
		
		return NO;
	}
	
@end

#endif